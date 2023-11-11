// Inverted pendulum example with codegen, the code is generated in `generated_code` directory, build and run it to see the result.
// Reference: https://ctms.engin.umich.edu/CTMS/index.php?example=InvertedPendulum&section=SystemModeling

#include <iostream>

#include <tinympc/admm.hpp>
#include <tinympc/codegen.hpp>

// For codegen, double type should be used, otherwise, Riccati won't converge.

extern "C"
{
    // Model parameters
    const tinytype mc = 0.5;
    const tinytype mp = 0.2;
    const tinytype l = 0.3;
    const tinytype b = 0.1;
    const tinytype g = 9.8;
    const tinytype I = 0.006;

    const int n = 4;  // state dimension
    const int m = 1;  // input dimension
    const int N = 10; // horizon

    const tinytype a11 = (-I + mp * l * l) * b / (I * mc + I * mp + mc * mp * l * l);
    const tinytype a12 = mp * mp * g * l * l / (I * mc + I * mp + mc * mp * l * l);
    const tinytype a31 = -mp * l * b / (I * mc + I * mp + mc * mp * l * l);
    const tinytype a32 = mp * g * l * (mp + mc) / (I * mc + I * mp + mc * mp * l * l);
    const tinytype b1 = (I + mp * l * l) / (I * mc + I * mp + mc * mp * l * l);
    const tinytype b3 = mp * l / (I * mc + I * mp + mc * mp * l * l);

    // Model matrices
    tinytype Adyn_data[n * n] = {0, 1, 0, 1,
                                 0, a11, a12, 0,
                                 0, 0, 0, 1,
                                 0, a31, a32, 0}; // Row-major
    tinytype Bdyn_data[n * m] = {0, b1, 0, b3};

    // Cost matrices
    tinytype Q_data[n] = {1, 0, 1, 0};
    tinytype Qf_data[n] = {10, 0, 10, 0};
    tinytype R_data[m] = {1};
    tinytype rho_value = 0.1;

    // Constraints
    tinytype x_min_data[n * N] = {-10};
    tinytype x_max_data[n * N] = {10};
    tinytype u_min_data[m * (N - 1)] = {-10};
    tinytype u_max_data[m * (N - 1)] = {10};

    // Solver options
    tinytype abs_pri_tol = 1e-3;
    tinytype rel_pri_tol = 1e-3;
    int max_iter = 100;
    int verbose = 1; // for code-gen

    // char tinympc_dir[255] = "your absolute path to tinympc";
    char tinympc_dir[255] = "/home/khai/SSD/Code/TinyMPC";
    char output_dir[255] = "/generated_code";

    int main()
    {
        // Python will call this function with the above data
        tiny_codegen(n, m, N, Adyn_data, Bdyn_data, Q_data, Qf_data, R_data, x_min_data, x_max_data, u_min_data, u_max_data, rho_value, abs_pri_tol, rel_pri_tol, max_iter, verbose, tinympc_dir, output_dir);

        return 0;
    }

} /* extern "C" */


/*

#include <iostream>

#include <tinympc/admm.hpp>
#include <tinympc/tiny_data_workspace.hpp>

using namespace Eigen;
IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

#ifdef __cplusplus
extern "C"
{
#endif

	int main()
	{
		int exitflag = 1;
		std::cout << tiny_data_solver.settings->max_iter << std::endl;
		std::cout << tiny_data_solver.cache->AmBKt.format(CleanFmt) << std::endl;
		std::cout << tiny_data_solver.work->Adyn.format(CleanFmt) << std::endl;

		exitflag = tiny_solve(&tiny_data_solver);

		if (exitflag == 0)
			printf("HOORAY! Solved with no error!\n");
		else
			printf("OOPS! Something went wrong!\n");

		TinyWorkspace work = *(tiny_data_solver.work); // for convenience
		tiny_VectorNx x0, x1; // current and next simulation states

		// Upright set point
		tiny_VectorNx Xref_origin;
		Xref_origin << 0, 0, M_PI, 0;
		
		work.Xref = Xref_origin.replicate<1, NHORIZON>();

		// Initial state
		x0 << 0, 0.1, M_PI - 0.1, 0.1;

		for (int k = 0; k < 70; ++k)
		{
			printf("tracking error at step %2d: %.4f\n", k, (x0 - work.Xref.col(1)).norm());

			// 1. Update measurement
			work.x.col(0) = x0;

			// 2. Update reference (if needed)

			// 3. Reset dual variables (if needed)
			work.y = tiny_MatrixNuNhm1::Zero();
			work.g = tiny_MatrixNxNh::Zero();

			// 4. Solve MPC problem
			tiny_solve(&tiny_data_solver);

			// std::cout << work.iter << std::endl;
			// std::cout << work.u.col(0).transpose().format(CleanFmt) << std::endl;

			// 5. Simulate forward
			x1 = work.Adyn * x0 + work.Bdyn * work.u.col(0);
			x0 = x1;

			// std::cout << x0.transpose().format(CleanFmt) << std::endl;
		}

		return 0;
	}

#ifdef __cplusplus
} 
#endif

*/