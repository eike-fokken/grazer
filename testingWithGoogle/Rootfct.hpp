#include <memory>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

Eigen::Vector2d Rootfct(Eigen::VectorXd &rootvalue, double last_time,
                             double new_time, Eigen::VectorXd const &last_state,
                             Eigen::VectorXd const &new_state){

    Eigen::Matrix2d A;
    A << 2,1,
         0,3;
    Eigen::Vector2d b(1,0);
    
    return rootvalue = A*new_state+b;

}