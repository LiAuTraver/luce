#pragma once
#include "config.hpp"
#include <accat/auxilia/auxilia.hpp>
namespace accat::luce {
class Timer {
public:
  Timer() : global_start_time_(clock_type::now()) {}
  Timer(const Timer &) = delete;
  Timer &operator=(const Timer &) = delete;
  Timer(Timer &&) noexcept = default;
  Timer &operator=(Timer &&) noexcept = default;
  ~Timer() = default;

public:
  using clock_type = std::chrono::high_resolution_clock;
  using duration_type = std::chrono::duration<double>;
  using time_point = std::chrono::time_point<clock_type>;
  using time_frame_type = std::tuple<time_point, time_point, duration_type>;
  using time_stack_type = std::deque<time_frame_type>;

public:
  template <typename Func, typename... Args>
    requires std::invocable<Func, Args...>
  auto measure(Func &&func, Args &&...args) noexcept(noexcept(
      std::invoke(std::forward<Func>(func), std::forward<Args>(args)...)))
      -> std::pair<std::invoke_result_t<Func, Args...>, duration_type> {
    this->start();
    auto res =
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    auto elapsedTime = this->stop();
    return {std::move(res), std::move(elapsedTime)};
    // return res;
  }

  duration_type lifetime() const {
    return std::chrono::duration_cast<duration_type>(clock_type::now() -
                                                     global_start_time_);
  }

private:
  void start() {
    precondition(current_frame_ == time_frame_type{},
                 "Timer is already started.")
    current_frame_ =
        std::make_tuple(clock_type::now(), time_point{}, duration_type{});
  }
  duration_type stop() {
    precondition(current_frame_ != time_frame_type{}, "Timer is not started.")

    time_stack_.emplace_back(current_frame_);
    current_frame_ = time_frame_type{};

    auto &[startTime, endTime, elapsedTime] = time_stack_.back();

    if (time_point_ == time_point{}) {
      endTime = clock_type::now();
      elapsedTime =
          std::chrono::duration_cast<duration_type>(endTime - startTime);
      return elapsedTime;
    }
    // debugger paused the timer; the `endTime` is the resume time,
    auto nowTime = clock_type::now();
    // time_point_ is the pause time
    elapsedTime = std::chrono::duration_cast<duration_type>(
        time_point_ - startTime + nowTime - endTime);
    endTime = nowTime;

    time_point_ = time_point{};
    return elapsedTime;
  }

  // function for debugger to pause the timer
  void pause() {
    precondition(current_frame_ != time_frame_type{}, "Timer is not started.")
    time_point_ = clock_type::now();
  }

  // function for debugger to resume the timer
  void resume() {
    precondition(current_frame_ != time_frame_type{}, "Timer is not started.")
    auto &[startTime, endTime, elapsedTime] = current_frame_;
    // use it to store the resume timepoint
    endTime = clock_type::now();
  }

private:
  time_point global_start_time_{};
  time_stack_type time_stack_{};
  time_frame_type current_frame_{};
  duration_type real_time_ = duration_type::zero();
  /// @brief time_point when the timer is paused
  time_point time_point_{};
};
} // namespace accat::luce
