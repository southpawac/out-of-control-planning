///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 4
// Authors: Aidan Curtis & Patrick Han
//////////////////////////////////////

#include <iostream>

#include <ompl/base/ProjectionEvaluator.h>

#include <ompl/control/SimpleSetup.h>
#include <ompl/control/ODESolver.h>

#include <ompl/control/spaces/RealVectorControlSpace.h>
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/base/spaces/SO2StateSpace.h>

// Your implementation of RG-RRT
#include "RG-RRT.h"

#include <cmath>

const double GRAVITY = 9.81;

// Your projection for the pendulum
class PendulumProjection : public ompl::base::ProjectionEvaluator
{
public:
    PendulumProjection(const ompl::base::StateSpace *space) : ProjectionEvaluator(space)
    {
    }

    unsigned int getDimension() const override
    {
        // TODO: The dimension of your projection for the pendulum
        return 0;
    }

    void project(const ompl::base::State */* state */, Eigen::Ref<Eigen::VectorXd> /* projection */) const override
    {
        // TODO: Your projection for the pendulum
    }
};

void pendulumODE(const ompl::control::ODESolver::StateType & q, const ompl::control::Control * c,
                 ompl::control::ODESolver::StateType & qdot)
{
    // TODO: Fill in the ODE for the pendulum's dynamics
    // Retreive Orientation/Control Value Omega
    const double *u = c->as<ompl::control::RealVectorControlSpace::ControlType>()->values;
    const double theta = u[0]; // Angle of pendulum
    const double omega = u[1]; // Angular velocity

    const double torque = 3; // Not sure how this should be passed in 

    qdot.resize(q.size(), 0); // Initialize qdot as zeros

    qdot[0] = omega;
    qdot[1] = -1 * GRAVITY * cos(theta) + torque;
}

ompl::control::SimpleSetupPtr createPendulum(double torque)
{
    // TODO: Create and setup the pendulum's state space, control space, validity checker, everything you need for
    // planning.
    // return nullptr;

    // STATE SPACE SETUP
    ompl::base::StateSpacePtr r1so2;

    // Create R^1 component of the State Space (angular velocity omega)
    auto r1 = std::make_shared<ompl::base::RealVectorStateSpace>(1);

    // Set bounds on R^1
    ompl::base::RealVectorBounds bounds(1);
    bounds.setLow(-10);  // omega (rotational velocity) has a min of -10
    bounds.setHigh(10);  // omega (rotational velocity) has a max of 10

    // Set the bounds on R^1
    r1->setBounds(bounds);

    // Create S1/SO(2) component of the state space
    auto so2 = std::make_shared<ompl::base::SO2StateSpace>();

    // Create compound state space (R^1 x SO(2))
    r1so2 = r1 + so2;


    // CONTROL SPACE SETUP
    auto controlSpace = std::make_shared<ompl::control::RealVectorControlSpace>(r1so2, 1); // Take in our state space, with 1 control
    
    // Set bounds on our control space torque
    ompl::base::RealVectorBounds cbounds(1);
    cbounds.setLow(-1 * abs(torque));
    cbounds.setHigh(abs(torque));
    controlSpace->setBounds(cbounds);

    // Define a simple setup class
    ompl::control::SimpleSetup ss(controlSpace);

    // Return simple setup
    ompl::control::SimpleSetupPtr ssptr(ss);
    return ssptr;

}

void planPendulum(ompl::control::SimpleSetupPtr & ss, int /* choice */)
{
    // TODO: Do some motion planning for the pendulum
    // choice is what planner to use.

    auto cspace = ss->getControlSpace();
    auto space  = ss->getStateSpace();

    // Construct instance of space information from this control space
    auto si(std::make_shared<ompl::control::SpaceInformation>(space, cspace));


    // set state validity checker TODO: Define isStateValid
    si->setStateValidityChecker([&si](const ompl::base::State *state) { return isStateValid(si.get(), state); });

    // Set state propagation routine


    // Create start state
    ompl::base::ScopedState<> start(space);
    start[0] = 0; // Initial velocity
    start[1] = -1 * M_PI/2; // Initial position

    // Create goal state
    ompl::base::ScopedState<> goal(space);
    goal[0] = 0; // Goal velocity
    goal[1] = M_PI/2; // Goal position


    // Create problem instance
    auto pdef(std::make_shared<ompl::base::ProblemDefinition>(si));

    // Set the start and goal states
    pdef->setStartAndGoalStates(start, goal, 0.1);

    // Create planner for space


    // Set the problem we are trying to solve for the planner


    // Perform setup steps for the planner

    


}

void benchmarkPendulum(ompl::control::SimpleSetupPtr &/* ss */)
{
    // TODO: Do some benchmarking for the pendulum
}

int main(int /* argc */, char ** /* argv */)
{
    int choice;
    do
    {
        std::cout << "Plan or Benchmark? " << std::endl;
        std::cout << " (1) Plan" << std::endl;
        std::cout << " (2) Benchmark" << std::endl;

        std::cin >> choice;
    } while (choice < 1 || choice > 2);

    int which;
    do
    {
        std::cout << "Torque? " << std::endl;
        std::cout << " (1)  3" << std::endl;
        std::cout << " (2)  5" << std::endl;
        std::cout << " (3) 10" << std::endl;

        std::cin >> which;
    } while (which < 1 || which > 3);

    double torques[] = {3., 5., 10.};
    double torque = torques[which - 1];

    ompl::control::SimpleSetupPtr ss = createPendulum(torque);

    // Planning
    if (choice == 1)
    {
        int planner;
        do
        {
            std::cout << "What Planner? " << std::endl;
            std::cout << " (1) RRT" << std::endl;
            std::cout << " (2) KPIECE1" << std::endl;
            std::cout << " (3) RG-RRT" << std::endl;

            std::cin >> planner;
        } while (planner < 1 || planner > 3);

        planPendulum(ss, planner);
    }
    // Benchmarking
    else if (choice == 2)
        benchmarkPendulum(ss);

    else
        std::cerr << "How did you get here? Invalid choice." << std::endl;

    return 0;
}