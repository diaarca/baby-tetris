#include "Action.h"
#include "MDP.h"
#include "State.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#define WIDTH 4
#define HEIGHT 4
#define EPSILON 0.00000001
#define MAX_IT 1000
#define ACTION_POLICY_LAMBDA 0.9
#define TROMINO_POLICY_LAMBDA 0.1
#define NB_SIMU 100

// Struct to hold the results for one configuration
struct RunResult
{
    int config_index;
    std::array<double, 4> params;
    double random_score;
    double minmax_score;
    double minavg_score;
    double gapavg_score;
};

int main()
{
    // initialize pseudo random generator
    srand((time(NULL) & 0xFFFF));

    // Initializing the game and the MDP
    Field master_field(WIDTH, HEIGHT);
    Game master_game(master_field);
    MDP master_mdp(WIDTH, HEIGHT, master_game.getState().clone());

    // --- Adversary Policy Computations ---
    std::cout << "Computing adversary policies..." << std::endl;
    std::unordered_map<State, std::unique_ptr<Tromino>> rand_tromino;
    std::unordered_map<State, std::unique_ptr<Tromino>> minmax_tromino =
        master_mdp.trominoValueIterationMinMax(EPSILON, MAX_IT,
                                               TROMINO_POLICY_LAMBDA);
    std::unordered_map<State, std::unique_ptr<Tromino>> minavg_tromino =
        master_mdp.trominoValueIterationMinAvg(EPSILON, MAX_IT,
                                               TROMINO_POLICY_LAMBDA);
    std::unordered_map<State, std::unique_ptr<Tromino>> gapavg_tromino =
        master_mdp.trominoValueIterationGapAvg(EPSILON, MAX_IT,
                                               TROMINO_POLICY_LAMBDA);
    std::cout << "Adversary policies computed." << std::endl << std::endl;

    // --- Configuration Exploration ---
    std::cout << "--- Starting Configuration Exploration ---" << std::endl;

    const std::vector<double> line_comp_weights = {0};
    const std::vector<double> height_weights = {0};
    const std::vector<double> score_weights = {1};
    const std::vector<double> gap_red_weights = {0};

    std::vector<std::array<double, 4>> configurations;
    for (double lw : line_comp_weights)
    {
        for (double hw : height_weights)
        {
            for (double sw : score_weights)
            {
                for (double gw : gap_red_weights)
                {
                     configurations.push_back({lw, hw, sw, gw});
                }
            }
        }
    }

    std::vector<RunResult> all_results;
    int config_idx = 0;

    for (const auto& config : configurations)
    {
        double line_w = config[0];
        double height_w = config[1];
        double score_w = config[2];
        double gap_r = config[3];

        std::cout << "Running configuration #" << config_idx << "/"
                  << configurations.size() << ": {" << line_w << ", "
                  << height_w << ", " << score_w << ", " << gap_r << "}"
                  << std::endl;

        std::unordered_map<State, Action> policy =
            master_mdp.actionValueIteration(ACTION_POLICY_LAMBDA, line_w,
                                            height_w, score_w, gap_r, EPSILON,
                                            MAX_IT);

        double rand_score_sum = 0;
        for (int i = 0; i < NB_SIMU; ++i)
        {
            rand_score_sum +=
                master_mdp.playPolicy(master_game, policy, rand_tromino);
        }

        all_results.push_back(
            {config_idx, config, rand_score_sum / NB_SIMU,
             (double)master_mdp.playPolicy(master_game, policy, minmax_tromino),
             (double)master_mdp.playPolicy(master_game, policy, minavg_tromino),
             (double)master_mdp.playPolicy(master_game, policy,
                                           gapavg_tromino)});

        config_idx++;
    }

    std::cout << "\n--- All Configurations Tested ---" << std::endl;

    // --- Final Results Table ---
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+"
              << std::endl;
    std::cout << "| Config | Line W. | Height W.| Score W.| Gap Red. | Rand "
                 "Avg | MinMax S.| MinAvg S.| GapAvg S.|"
              << std::endl;
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+----------+"
              << std::endl;

    for (const auto& result : all_results)
    {
        std::cout << "| #" << std::setw(5) << std::left << result.config_index
                  << "| " << std::setw(7) << std::left << result.params[0]
                  << "| " << std::setw(8) << std::left << result.params[1]
                  << "| " << std::setw(7) << std::left << result.params[2]
                  << "| " << std::setw(8) << std::left << result.params[3]
                  << "| " << std::setw(8) << std::left << std::fixed
                  << std::setprecision(2) << result.random_score << "| "
                  << std::setw(8) << std::left << result.minmax_score << "| "
                  << std::setw(8) << std::left << result.minavg_score << "| "
                  << std::setw(8) << std::left << result.gapavg_score << " |"
                  << std::endl;
    }
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+----------+"
              << std::endl;

    return 0;
}
