#include "deps.hh"

#include "luce/repl/ExprVisitor.hpp"
#include "luce/repl/Debugger.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
namespace accat::luce::repl {
using enum auxilia::FormatPolicy;
using auxilia::FormatPolicy;
using auxilia::NotFoundError;
using auxilia::OkStatus;
using auxilia::println;
using auxilia::Status;
using auxilia::StatusOr;

auto WatchPoint::to_string(const FormatPolicy policy) const -> string_type {
  if (policy == kDetailed)
    return fmt::format("WatchPoint {}: expression {}, AST {}",
                       id_,
                       expr_,
                       AST_->to_string(policy));
  else if (policy == kDefault)
    return fmt::format("WatchPoint {}: {}", id_, expr_);
  else
    return expr_;
}
auto WatchPoint::update(expression::Visitor *visitor)
    -> std::pair<bool, result_type> {

  using RetType = std::pair<bool, result_type>;

  return *AST_->accept(*visitor)
              // result is valid
              .transform([this](auto &&res) -> RetType {
                if (!previous_result_) {
                  // previous result is invalid, update it and return true
                  previous_result_ = res;
                  return {true, res};
                }
                if (*previous_result_ == res or previous_result_->empty()) {
                  // both valid and equal, or no previous result, return false
                  return {false, res};
                }
                // both valid but not equal, update previous result and return
                // true
                previous_result_ = res;
                return {true, res};
              })
              // result is invalid, return false
              .transform_error(
                  [](auto &&res) -> RetType { return {false, res}; });
}
auto WatchPoints::to_string(const FormatPolicy policy) const -> string_type {
  auto str = string_type{};
  if (policy == kDetailed) {
    str += "WatchPoints: [";
    str += fmt::format("{}", fmt::join(watchpoints_, "\n"));
    str += fmt::format("]\nTotal watchpoints: {}", watchpoints_.size());
  } else if (policy == kDefault) {
    str += fmt::format("WatchPoints: [");
    std::ranges::for_each(watchpoints_, [&str](const auto &wp) {
      str += fmt::format("{}, ", wp.to_string(kDefault));
    });
    str += "]";
  } else {
    if (watchpoints_.empty()) {
      str += "No watchpoints";
    } else {
      str += fmt::format("WatchPoints: {}",
                         watchpoints_.front().to_string(kBrief));
      std::ranges::for_each(std::ranges::next(std::ranges::begin(watchpoints_)),
                            std::ranges::end(watchpoints_),
                            [&str](const auto &wp) {
                              str += fmt::format(", {}", wp.to_string(kBrief));
                            });
    }
  }
  return str;
}
bool WatchPoints::update(expression::Visitor *visitor, const bool logging) {
  bool has_changed = false;
  std::ranges::for_each(watchpoints_, [&](auto &wp) {
    auto [changed, res] = wp.update(visitor);
    if (changed) {
      // if has changed, the res is guranateed to be valid
      has_changed = true;
      if (logging)
        spdlog::info(
            "WatchPoint {}: {}", wp.id(), res->underlying_string(kBrief));
      else
        spdlog::debug(
            "WatchPoint {}: {}", wp.id(), res->underlying_string(kBrief));
    } else if (logging) {
      spdlog::info("WatchPoint {}: no change", wp.id());
    } else {
      spdlog::trace("WatchPoint {}: no change", wp.id());
    }
  });
  return has_changed;
}
Debugger::Debugger(Mediator *parent)
    : Component(parent), watchpoints_{},
      visitor_{new expression::Evaluator{parent}} {}
Debugger::Debugger(Debugger &&that) noexcept = default;
Debugger &Debugger::operator=(Debugger &&that) noexcept = default;
Debugger::~Debugger() = default;

Status Debugger::add_watchpoint(const std::string &expr) {
  return Parser{Lexer{}.load_string(expr).lex()}
      .next_expression()
      // parse success
      .transform([&, this](auto &&AST) {
        AST->accept(*visitor_)
            .transform([](auto &&res) {
              fmt::println(stdout,
                           "Current evaluation result: {result}",
                           "result"_a = res.underlying_string(kBrief));
            })
            .transform_error([](auto &&res) {
              spdlog::warn("Watchpoint added, but "
                           "evaluation failed: {msg}",
                           "msg"_a = res.message());
            });
        this->watchpoints_.add(expr, std::move(AST));
      })
      // parse error
      .transform_error([](auto &&res) {
        println(stderr,
                fg(fmt::color::orange),
                "Cannot add watchpoint: {msg}",
                "msg"_a = res.message());
      });
}
Status Debugger::delete_watchpoint(const size_t id) {
  return watchpoints_.remove(id) ? OkStatus()
                                 : NotFoundError("Watchpoint not found");
}
void Debugger::update_watchpoints(const bool notify, const bool logging) {
  if (watchpoints_.update(visitor_, logging) && notify) {
    this->send(Event::kPauseTask);
  }
}
} // namespace accat::luce::repl
