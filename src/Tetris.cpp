#include "Action.h"
#include "MDP.h"
#include "State.h"
#include <algorithm> // For std::sort
#include <cstdlib>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip> // For std::fixed, std::setprecision
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#define WIDTH 4
#define HEIGHT 4
#define EPSILON 0.00000001
#define MAX_IT 1000
#define ACTION_POLICY_LAMBDA 0.9
#define TROMINO_POLICY_LAMBDA 0.1
#define NB_SIMU 10

// Struct to hold the result of a single grid search evaluation
struct GridSearchResult
{
    double avg_score;
    double line_weight;
    double height_weight;
    double score_weight;
};

// Self-contained function to evaluate a set of weights.
// It creates its own MDP and Game objects to be thread-safe.
GridSearchResult evaluate_weights(double lw, double hw, double sw, State s0)
{
    Field field(WIDTH, HEIGHT);
    Game game(field);
    MDP mdp(WIDTH, HEIGHT, s0.clone());
    std::unordered_map<State, std::unique_ptr<Tromino>> rand_tromino;

    std::unordered_map<State, Action> temp_ff_policy = mdp.fullFeaturePolicy(
        ACTION_POLICY_LAMBDA, lw, hw, sw, EPSILON, MAX_IT);

    double current_avg_score = 0;
    for (int i = 0; i < NB_SIMU; ++i)
    {
        current_avg_score += mdp.playPolicy(game, temp_ff_policy, rand_tromino);
    }
    current_avg_score /= NB_SIMU;

    return {current_avg_score, lw, hw, sw};
}

int main()
{
    // initialize pseudo random generator
    srand((time(NULL) & 0xFFFF));

    std::cout << "\n";

    // Create a master MDP to get the initial state and run final comparisons
    Field master_field(WIDTH, HEIGHT);
    Game master_game(master_field);
    MDP master_mdp(WIDTH, HEIGHT, master_game.getState().clone());
    State s0 = master_game.getState().clone();

    // --- Hyperparameter Grid Search ---
    // std::cout << "--- Starting Parallel Grid Search for Full Feature Policy "
    //              "Weights ---"
    //           << std::endl;
    // std::cout << "Launching " << std::thread::hardware_concurrency()
    //           << " concurrent threads if available." << std::endl;
    //
    // std::vector<double> line_weights_to_test = {5.0, 10.0, 20.0};
    // std::vector<double> height_weights_to_test = {1.0, 2.0, 4.0};
    // std::vector<double> score_weights_to_test = {0.5, 1.0, 2.0};
    //
    // std::vector<std::future<GridSearchResult>> futures;
    //
    // for (double lw : line_weights_to_test)
    // {
    //     for (double hw : height_weights_to_test)
    //     {
    //         for (double sw : score_weights_to_test)
    //         {
    //             futures.push_back(std::async(std::launch::async,
    //                                          evaluate_weights, lw, hw, sw,
    //                                          s0.clone()));
    //         }
    //     }
    // }
    //
    // std::vector<GridSearchResult> all_results;
    // for (auto& fut : futures)
    // {
    //     all_results.push_back(fut.get());
    // }
    //
    // // Sort results from best to worst
    // std::sort(all_results.begin(), all_results.end(),
    //           [](const GridSearchResult& a, const GridSearchResult& b)
    //           { return a.avg_score > b.avg_score; });
    //
    // std::cout << "\n--- Grid Search Complete ---" << std::endl;
    // std::cout
    //     << "+---------------+----------------+---------------+---------------+"
    //     << std::endl;
    // std::cout
    //     << "|  Line Weight  | Height Weight  |  Score Weight | Average Score |"
    //     << std::endl;
    // std::cout
    //     << "+---------------+----------------+---------------+---------------+"
    //     << std::endl;
    // for (const auto& result : all_results)
    // {
    //     std::cout << "| " << std::setw(13) << std::left << result.line_weight
    //               << "| " << std::setw(14) << std::left << result.height_weight
    //               << "| " << std::setw(13) << std::left << result.score_weight
    //               << "| " << std::setw(13) << std::left << std::fixed
    //               << std::setprecision(2) << result.avg_score << " |"
    //               << std::endl;
    // }
    // std::cout
    //     << "+---------------+----------------+---------------+---------------+"
    //     << std::endl;
    //
    // double best_line_weight = all_results[0].line_weight;
    // double best_height_weight = all_results[0].height_weight;
    // double best_score_weight = all_results[0].score_weight;

    double best_line_weight = 10.0;
    double best_height_weight = 1;
    double best_score_weight = 2;

    // std::cout << "\nBest Combination:" << std::endl;
    // std::cout << "Best LINE_WEIGHT: " << best_line_weight << std::endl;
    // std::cout << "Best HEIGHT_WEIGHT: " << best_height_weight << std::endl;
    // std::cout << "Best SCORE_WEIGHT: " << best_score_weight << std::endl;
    // std::cout << "Best Average Score: " << all_results[0].avg_score << std::endl
    //           << std::endl;

    // --- Final Comparison with Best Weights ---
    std::cout << "--- Running Final Comparison with Best Weights ---"
              << std::endl;

    std::unordered_map<State, Action> best_ff_policy =
        master_mdp.fullFeaturePolicy(ACTION_POLICY_LAMBDA, best_line_weight,
                                     best_height_weight, best_score_weight,
                                     EPSILON, MAX_IT);

    std::unordered_map<State, std::unique_ptr<Tromino>> rand_tromino;
    std::unordered_map<State, std::unique_ptr<Tromino>> minmax_tromino =
        master_mdp.trominoValueIterationMinMax(EPSILON, MAX_IT,
                                               TROMINO_POLICY_LAMBDA);
    std::unordered_map<State, std::unique_ptr<Tromino>> minavg_tromino =
        master_mdp.trominoValueIterationMinAvg(EPSILON, MAX_IT,
                                               TROMINO_POLICY_LAMBDA);

    double final_rand_score = 0;
    for (int i = 0; i < NB_SIMU * 5;
         ++i) // Run more simulations for the final result
    {
        final_rand_score +=
            master_mdp.playPolicy(master_game, best_ff_policy, rand_tromino);
    }
    double final_minmax_score =
        master_mdp.playPolicy(master_game, best_ff_policy, minmax_tromino);
    double final_minavg_score =
        master_mdp.playPolicy(master_game, best_ff_policy, minavg_tromino);

    std::cout << "\n--- Final Results for Best Full Feature Policy ---"
              << std::endl;
    std::cout << "Random Adversary: " << final_rand_score / (NB_SIMU * 5)
              << std::endl;
    std::cout << "MinMax Adversary: " << final_minmax_score << std::endl;
    std::cout << "MinAvg Adversary: " << final_minavg_score << std::endl;

    return 0;
}
