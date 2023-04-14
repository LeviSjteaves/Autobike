#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#define g 9.81;


/**
 * Trajectory controller
 * 
 * Input:
 * @param traj          Trajectory reference [X_ref Y_ref Psi_ref]
 * @param X_est         X position estimated state from kalman filter
 * @param Y_est         Y position estimated state from kalman filter
 * @param Psi_est       Heading estimated state from kalman filter
 * 
 * Output:
 * @param Roll_ref      Roll reference fot the balance control
 * 
 * Parameters: 
 * @param bike_params             Gravity parameter     [g lr lf lambda]
 * @param traj_params             Trajectory parameters [k1 k2 e1_max]
 * @param v                       Velocity
 * 
 * 
 */


// Main function

// extern void trajectory_controller(double *traj_loc, double X_est, double Y_est, double Psi_est, double v, double *bike_params, double *traj_params, double *roll_ref,
//                                     int *closestpoint_idx)
// {
//     // unpack parameters
//     double lr, lf,lambda;
//     lr = bike_params[0];
//     lf = bike_params[1];
//     lambda = bike_params[2];

//     double k1,k2,e1_max;
//     k1 = traj_params[0];
//     k2 = traj_params[1];
//     e1_max = traj_params[2];

//     // Unpack the trajectory
//     int size_traj_loc = sizeof(traj_loc) / 3; // length of the trajectory
//     double X_loc[size_traj_loc];
//     double Y_loc[size_traj_loc];
//     double Psi_loc[size_traj_loc];
//     int counter = 0;

//     for (int i = 0; i < size_traj_loc; i++)
//     {
//         X_loc[counter] = traj_loc[i];
//         Y_loc[counter] = traj_loc[i + size_traj_loc];
//         Psi_loc[counter] = traj_loc[i + 2*size_traj_loc];
//         counter += 1;
//     }

//     // Second point in traj_loc is current selected closest point
//     closestpoint_idx = 1;   

//     // Search for closest point (find the closest point going forward, stop when distance increases)
//     while (pow(X_loc[closestpoint_idx]-X_est,2.0) + pow(Y_loc[closestpoint_idx]-Y_est,2.0) >= 
//             pow(X_loc[closestpoint_idx+1]-X_est,2.0) + pow(Y_loc[closestpoint_idx+1]-Y_est,2.0) && closestpoint_idx <= size_traj_loc-1)
//         {
//             closestpoint_idx += 1;
//         }

//     // select same closest point for heading and position error
//     int closestpoint_heading_idx = closestpoint_idx;
 
//     // Compute X and Y distance from current location to selected closest point
//     double dx,dy;
//     dx = X_est - X_loc[closestpoint_idx];
//     dy = Y_est - Y_loc[closestpoint_idx];

//     // Compute difference from current heading and heading reference points





// }

