#include <memory>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include "Matrixhandler.hpp"

void DevRootfct(Aux::Matrixhandler *jacobianhandler,
                           double last_time, double new_time,
                           Eigen::VectorXd const &last_state,
                           Eigen::VectorXd const &new_state){

    Eigen::SparseMatrix<double> matrix(2,2);
    matrix.coeffRef(0,0)=2.;
    matrix.coeffRef(0,1)=1.;
    matrix.coeffRef(1,1)=3.;
    Eigen::SparseMatrix<double> *matrix2 = &matrix;
  
    //Aux::Triplethandler triplet(matrix2); 


}