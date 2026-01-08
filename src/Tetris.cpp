#include "Action.h"
#include "MDP.h"
#include "State.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#define WIDTH 4
#define HEIGHT 4
#define EPSILON 0.00000001
#define MAX_IT 1000
#define ACTION_POLICY_LAMBDA 0.9
#define TROMINO_POLICY_LAMBDA 0.1
#define NB_SIMU 100

// --- Global Data Structures ---

// Struct to hold the results for one configuration
struct RunResult
{
    int config_index;
    std::array<double, 4> params;
    double random_score;
    double minmax_score;
    double minavg_score;
    double gapavg_score;
    double min_score;
};

// Global adversary policies to be accessible by all threads (read-only)
std::unordered_map<State, std::unique_ptr<Tromino>> g_rand_tromino;
std::unordered_map<State, std::unique_ptr<Tromino>> g_minmax_tromino;
std::unordered_map<State, std::unique_ptr<Tromino>> g_minavg_tromino;
std::unordered_map<State, std::unique_ptr<Tromino>> g_gapavg_tromino;

// Global mutex to protect console output
std::mutex g_cout_mutex;

// --- Thread-safe evaluation function ---

RunResult evaluate_configuration(int idx, std::array<double, 4> p, State s0)
{
    double line_w = p[0];
    double height_w = p[1];
    double score_w = p[2];
    double gap_r = p[3];

    // Each thread gets its own MDP and Game objects
    Field field(WIDTH, HEIGHT);
    Game game(field);
    MDP mdp(WIDTH, HEIGHT, s0.clone());

    {
        std::lock_guard<std::mutex> lock(g_cout_mutex);
        std::cout << "Running configuration #" << idx << ": {" << line_w << ", "
                  << height_w << ", " << score_w << ", " << gap_r << "}"
                  << std::endl;
    }

    std::unordered_map<State, Action> policy =
        mdp.actionValueIteration(ACTION_POLICY_LAMBDA, line_w, height_w,
                                 score_w, gap_r, EPSILON, MAX_IT);

    double rand_score_sum = 0;
    for (int i = 0; i < NB_SIMU; ++i)
    {
        rand_score_sum += mdp.playPolicy(game, policy, g_rand_tromino);
    }
    double rand_avg = rand_score_sum / NB_SIMU;
    double minmax_score =
        (double)mdp.playPolicy(game, policy, g_minmax_tromino);
    double minavg_score =
        (double)mdp.playPolicy(game, policy, g_minavg_tromino);
    double gapavg_score =
        (double)mdp.playPolicy(game, policy, g_gapavg_tromino);

    double min_score =
        std::min({rand_avg, minmax_score, minavg_score, gapavg_score});

    return {idx,          p,        rand_avg, minmax_score, minavg_score,
            gapavg_score, min_score};
}

// --- Analysis Helper Functions ---
double calculate_variance(const RunResult& res)
{
    double mean = (res.random_score + res.minmax_score + res.minavg_score +
                   res.gapavg_score) /
                  4.0;
    double sq_diff = 0;
    sq_diff += std::pow(res.random_score - mean, 2);
    sq_diff += std::pow(res.minmax_score - mean, 2);
    sq_diff += std::pow(res.minavg_score - mean, 2);
    sq_diff += std::pow(res.gapavg_score - mean, 2);
    return sq_diff / 4.0;
}

void print_result_summary(const std::string& title, const RunResult& result)
{
    std::cout << "--- " << title << " ---" << std::endl;
    std::cout << "  Configuration #" << result.config_index << ": {"
              << result.params[0] << ", " << result.params[1] << ", "
              << result.params[2] << ", " << result.params[3] << "}"
              << std::endl;
    std::cout << "  Scores -> Random: " << std::fixed << std::setprecision(2)
              << result.random_score << " | MinMax: " << result.minmax_score
              << " | MinAvg: " << result.minavg_score
              << " | GapAvg: " << result.gapavg_score
              << " | Min Score: " << result.min_score << std::endl
              << std::endl;
}

int main()
{
    srand((time(NULL) & 0xFFFF));

    Field master_field(WIDTH, HEIGHT);
    Game master_game(master_field);
    MDP master_mdp(WIDTH, HEIGHT, master_game.getState().clone());
    State s0 = master_game.getState().clone();

    std::cout << "Computing adversary policies..." << std::endl;
    // Initialize global adversary policies
    g_rand_tromino =
        std::unordered_map<State,
                           std::unique_ptr<Tromino>>(); // Empty map for random
    g_minmax_tromino = master_mdp.trominoValueIterationMinMax(
        EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);
    g_minavg_tromino = master_mdp.trominoValueIterationMinAvg(
        EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);
    g_gapavg_tromino = master_mdp.trominoValueIterationGapAvg(
        EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);
    std::cout << "Adversary policies computed." << std::endl << std::endl;

    std::cout << "--- Starting Parallel Configuration Exploration ---"
              << std::endl;
    std::cout << "Launching " << std::thread::hardware_concurrency()
              << " concurrent threads if available." << std::endl;

    const std::vector<double> line_weights = {0.0};
    const std::vector<double> height_weights = {0.0};
    const std::vector<double> score_weights = {1.0};
    const std::vector<double> gap_reduction_weights = {0.0};

    std::vector<std::future<RunResult>> futures;
    int config_idx = 0;

    for (double line_w : line_weights)
    {
        for (double height_w : height_weights)
        {
            for (double score_w : score_weights)
            {
                for (double gap_r : gap_reduction_weights)
                {
                    std::array<double, 4> params = {line_w, height_w, score_w,
                                                    gap_r};
                    futures.push_back(
                        std::async(std::launch::async, evaluate_configuration,
                                   config_idx, params, s0.clone()));
                    config_idx++;
                }
            }
        }
    }

    std::vector<RunResult> all_results;
    for (auto& fut : futures)
    {
        all_results.push_back(fut.get());
    }

    std::sort(all_results.begin(), all_results.end(),
              [](const RunResult& a, const RunResult& b)
              { return a.config_index < b.config_index; });

    std::cout << "\n--- All Configurations Tested ---" << std::endl;
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+----------+-----------+"
              << std::endl;
    std::cout << "| Config | Line W. | Height W.| Score W.| Gap Red. | Rand "
                 "Avg | MinMax S.| MinAvg S.| GapAvg S.| Min Score |"
              << std::endl;
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+----------+-----------+"
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
                  << std::setw(8) << std::left << result.gapavg_score << "| "
                  << std::setw(9) << std::left << result.min_score << " |"
                  << std::endl;
    }
    std::cout << "+--------+---------+----------+---------+----------+---------"
                 "-+----------+----------+----------+-----------+"
              << std::endl;

    if (!all_results.empty())
    {
        std::cout << "\n--- Detailed Analysis of Configurations ---"
                  << std::endl
                  << std::endl;

        auto best_rand =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.random_score < b.random_score; });
        print_result_summary("Best for Random Adversary", best_rand);

        auto worst_rand =
            *std::min_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.random_score < b.random_score; });
        print_result_summary("Worst for Random Adversary", worst_rand);

        auto best_minmax =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.minmax_score < b.minmax_score; });
        print_result_summary("Best for MinMax Adversary", best_minmax);

        auto worst_minmax =
            *std::min_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.minmax_score < b.minmax_score; });
        print_result_summary("Worst for MinMax Adversary", worst_minmax);

        auto best_minavg =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.minavg_score < b.minavg_score; });
        print_result_summary("Best for MinAvg Adversary", best_minavg);

        auto worst_minavg =
            *std::min_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.minavg_score < b.minavg_score; });
        print_result_summary("Worst for MinAvg Adversary", worst_minavg);

        auto best_gapavg =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.gapavg_score < b.gapavg_score; });
        print_result_summary("Best for GapAvg Adversary", best_gapavg);

        auto worst_gapavg =
            *std::min_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.gapavg_score < b.gapavg_score; });
        print_result_summary("Worst for GapAvg Adversary", worst_gapavg);

        auto most_robust =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              { return a.min_score < b.min_score; });
        print_result_summary("Most Robust (Highest Minimum Score)",
                             most_robust);

        auto most_variable = *std::max_element(
            all_results.begin(), all_results.end(),
            [](const RunResult& a, const RunResult& b)
            { return calculate_variance(a) < calculate_variance(b); });
        print_result_summary("Most Variable (Highest Score Variance)",
                             most_variable);

        auto best_overall =
            *std::max_element(all_results.begin(), all_results.end(),
                              [](const RunResult& a, const RunResult& b)
                              {
                                  return (a.random_score + a.minmax_score +
                                          a.minavg_score + a.gapavg_score) /
                                             4.0 <
                                         (b.random_score + b.minmax_score +
                                          b.minavg_score + b.gapavg_score) /
                                             4.0;
                              });
        print_result_summary("Best Overall (Average across all Adversaries)",
                             best_overall);
    }

    std::unordered_map<State, Action> robustPolicyMaxMin =
        master_mdp.robustActionValueIterationMaxMin(EPSILON, MAX_IT,
                                                    ACTION_POLICY_LAMBDA);

    std::cout << std::endl
              << std::endl
              << "In comparison, the MaxMin VI got these performances:"
              << std::endl;

    std::cout << "vs Random: "
              << master_mdp.playPolicy(master_game, robustPolicyMaxMin,
                                       g_rand_tromino)
              << std::endl;
    std::cout << "vs MinMax: "
              << master_mdp.playPolicy(master_game, robustPolicyMaxMin,
                                       g_minmax_tromino)
              << std::endl;
    std::cout << "vs MinAvg: "
              << master_mdp.playPolicy(master_game, robustPolicyMaxMin,
                                       g_minavg_tromino)
              << std::endl;
    std::cout << "vs GapAvg: "
              << master_mdp.playPolicy(master_game, robustPolicyMaxMin,
                                       g_gapavg_tromino)
              << std::endl;

    return 0;
}
