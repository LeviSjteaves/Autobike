#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define g 9.81

/**
 * Trajectory controller
 *
 * Input:
 * @param traj                  Trajectory reference [X_ref Y_ref Psi_ref]
 * @param X_est                 X position estimated state from kalman filter
 * @param Y_est                 Y position estimated state from kalman filter
 * @param Psi_est               Heading estimated state from kalman filter
 *
 * Output:
 * @param Roll_ref                Roll reference fot the balance control
 * @param closestpoint_idx_out
 *
 * Parameters:
 * @param bike_params             Gravity parameter     [g lr lf lambda]
 * @param traj_params             Trajectory parameters [k1 k2 e1_max]
 * @param v                       Velocity
 * @param Ad_t
 * @param Bd_t
 * @param C_t
 * @param D_t
 *
 */

// sign value for double
double sign(double value)
{
    if (value > 0.0)
    {
        return 1;
    }
    else if (value < 0.0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

// Min function for double
double min(double a, double b) 
{
    if (a < b)
    {
        return a;
    } 
    else
    {
        return b;
    }
}

// Modulo function (remainder of a division)
double mod(double a, double b) {
    double quotient = a / b;
    double quotient_floor = floor(quotient);
    double remainder = a - quotient_floor * b;
    return remainder;
}

// Wrap angle between pi and -pi
double wrap_angle(double angle) {
    double wrapped_angle = fmod(angle + M_PI, 2*M_PI);
    if (wrapped_angle < 0) {
        wrapped_angle += 2*M_PI;
    }
    return wrapped_angle - M_PI;
}

extern void trajectory_controller(double *traj_loc, double X_est, double Y_est, double Psi_est, double v, double*bike_params, 
                                    double *traj_params, double Ad_t, double Bd_t, double C_t, double D_t, double *roll_ref, int *closestpoint_idx_out)
{
    // unpack parameters
    double lr, lf,lambda;
    lr = bike_params[0];
    lf = bike_params[1];
    lambda = bike_params[2];

    double k1,k2,e1_max;
    k1 = traj_params[0];
    k2 = traj_params[1];
    e1_max = traj_params[2];

    // Unpack the trajectory
    int size_traj_loc = sizeof(traj_loc) / 3; // length of the trajectory
    double X_loc[size_traj_loc];
    double Y_loc[size_traj_loc];
    double Psi_loc[size_traj_loc];
    int counter = 0;

    for (int i = 0; i < size_traj_loc; i++)
    {
        X_loc[counter] = traj_loc[i];
        Y_loc[counter] = traj_loc[i + size_traj_loc];
        Psi_loc[counter] = traj_loc[i + 2*size_traj_loc];
        counter += 1;
    }

    // Second point in traj_loc is current selected closest point
    int closestpoint_idx = 1;

    // Search for closest point (find the closest point going forward, stop when distance increases)
    while (pow(X_loc[closestpoint_idx]-X_est,2.0) + pow(Y_loc[closestpoint_idx]-Y_est,2.0) >=
            pow(X_loc[closestpoint_idx+1]-X_est,2.0) + pow(Y_loc[closestpoint_idx+1]-Y_est,2.0) 
            && closestpoint_idx <= size_traj_loc-1)
    {
        closestpoint_idx += 1;
    }

    // select same closest point for heading and position error
    int closestpoint_heading_idx = closestpoint_idx;

    // Compute X and Y distance from current location to selected closest point
    double dx,dy;
    dx = X_est - X_loc[closestpoint_idx];
    dy = Y_est - Y_loc[closestpoint_idx];

    // Compute difference from current heading and heading reference points
    double D_psiref = Psi_loc[closestpoint_idx+1] - Psi_loc[closestpoint_idx];

    // Limit D_psiref between -pi and pi
    if (D_psiref >= M_PI)
    {
        D_psiref =  mod(D_psiref,-2*M_PI);
    }
    if (D_psiref <= -M_PI)
    {
        D_psiref =  mod(D_psiref,2*M_PI);
    }

    // Interpolation algorithm (IMPROVEMENT)
    // Compute distance between point before closespoint and current location
    double dis_true_previous = sqrt(pow(X_loc[closestpoint_idx-1]-X_est,2.0) + pow(Y_loc[closestpoint_idx-1]-Y_est,2.0));
    // Angle of line between previous closestpoint and current location
    double alpha_star = atan((Y_loc[closestpoint_idx-1]-Y_est)/(X_loc[closestpoint_idx-1]-X_est));
    // heading of previous closestpoint - alpha_star
    double alpha_projected_dist = alpha_star - Psi_loc[closestpoint_idx];
    // Distance from previous closestpoint to the projection of the bike in the trajectory
    double projected_dist = fabs(dis_true_previous*cos(alpha_projected_dist));
    double compared_dist = sqrt(pow(X_loc[closestpoint_idx]-X_loc[closestpoint_idx-1],2.0) + pow(Y_loc[closestpoint_idx]-Y_loc[closestpoint_idx-1],2.0));

    // When bike passses the closestpoint, heading from next point is taken
    if (projected_dist >= compared_dist)
    {
        closestpoint_heading_idx = closestpoint_idx + 1;
    }

    // Compute e1 and e2
    double e1 = dy*cos(Psi_loc[closestpoint_heading_idx]) - dx*sin(Psi_loc[closestpoint_heading_idx]);
    double e2 = Psi_est - Psi_loc[closestpoint_heading_idx];

    // Closestpoint in the local trajectory
    double ref_X = X_loc[closestpoint_idx];
    double ref_Y = Y_loc[closestpoint_idx];
    double ref_Psi = Psi_loc[closestpoint_heading_idx];

    // Keep heading error between -pi and pi
    e2 = wrap_angle(e2);

    // Compute time between psiref(idx) and psiref(idx+1)
    double dX = X_loc[closestpoint_idx+1] - ref_X;
    double dY = Y_loc[closestpoint_idx+1] - ref_Y;

    double dis = sqrt(pow(dX,2.0) + pow(dY,2.0));
    double Ts_psi = dis / v;

    // Sample dpsiref
    double dpsiref = D_psiref / Ts_psi;

    // Reset closespoint index as feedback for local selection
    closestpoint_idx = closestpoint_idx-1;
    *closestpoint_idx_out = closestpoint_idx; 

    // Steering contribution from trajectory error
    double delta_ref_error = -k1*sign(e1)*min(fabs(e1),e1_max) - k2*e2;

    // Steering contribution from heading change
    static double x = 0;
    double x_dot = 0;

    x_dot = Ad_t*x + Bd_t*dpsiref;
    double delta_ref_psi = C_t*x + D_t*dpsiref;

    x = x_dot;

    // Total steering reference
    double delta_ref = delta_ref_psi + delta_ref_error;

    // Limit steer angle reference between -pi/4 and pi/4
    if (delta_ref >= M_PI/4)
    {
        delta_ref = M_PI/4;
    }
    if (delta_ref <= -M_PI/4)
    {
        delta_ref = -M_PI/4;
    }
    
    // Transform steering reference into roll reference for the balance control
    double eff_delta_ref = delta_ref*sin(lambda);
    *roll_ref = -1*atan(tan(eff_delta_ref)*(pow(v,2.0)/(lr+lf))/g);

 }
