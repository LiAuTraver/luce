#pragma once

#include <utility>

#include "luce/repl/repl_fwd.hpp"
#include "luce/Support/utils/Pattern.hpp"

namespace accat::luce::repl {
namespace expression {
struct Visitor;
struct Expr;
} // namespace expression
class WatchPoint : public auxilia::Printable<WatchPoint> {
public:
  using expr_ptr_t = std::shared_ptr<expression::Expr>;
  WatchPoint(const size_t id, const std::string &expr, expr_ptr_t exprAST)
      : id_(id), expr_(expr), AST_(exprAST), previous_result_{} {}

public:
  string_type to_string(
      const auxilia::FormatPolicy policy = auxilia::FormatPolicy::kBrief) const;
  auto expr() const [[clang::lifetimebound]]
  -> auxilia::Viewable<WatchPoint>::string_view_type {
    return expr_;
  }
  auto id() const noexcept -> size_t {
    return id_;
  }
  auto AST() const noexcept -> expr_ptr_t {
    return AST_;
  }

private:
  using result_type = typename evaluation::result_type;

public:
  /// @return true if the result has changed; false otherwise; if the
  /// expression is invalid, always return false.
  /// also return the result of the expression
  auto update(expression::Visitor *) -> std::pair<bool, result_type>;

private:
  size_t id_;
  std::string expr_;
  expr_ptr_t AST_;
  result_type previous_result_;
};
class WatchPoints : public auxilia::Printable<WatchPoints> {
public:
  using watchpoint_t = WatchPoint;
  using watchpoints_t = std::vector<watchpoint_t>;

public:
  WatchPoints() = default;
  WatchPoints(const WatchPoints &other) = delete;
  WatchPoints(WatchPoints &&other) noexcept = default;
  WatchPoints &operator=(const WatchPoints &other) = delete;
  WatchPoints &operator=(WatchPoints &&other) noexcept = default;

  auto add(const std::string &expr, watchpoint_t::expr_ptr_t AST)
      -> WatchPoints & {
    watchpoints_.emplace_back(wp_id++, expr, AST);
    return *this;
  }
  auto remove(const size_t id) -> size_t {
    return std::erase_if(watchpoints_,
                         [id](const auto &wp) { return wp.id() == id; });
  }
  string_type to_string(
      const auxilia::FormatPolicy policy = auxilia::FormatPolicy::kBrief) const;

  bool update(expression::Visitor *visitor, bool);

private:
  watchpoints_t watchpoints_;
  static size_t wp_id;
};
inline size_t WatchPoints::wp_id = 0;

class Debugger : public Component {
public:
  explicit Debugger(Mediator *parent = nullptr);
  Debugger(const Debugger &) = delete;
  Debugger &operator=(const Debugger &) = delete;
  Debugger(Debugger &&) noexcept;
  Debugger &operator=(Debugger &&) noexcept ;
  virtual ~Debugger() override;

public:
  auto &watchpoints(this auto &&self) noexcept {
    return self.watchpoints_;
  }
  auxilia::Status add_watchpoint(const std::string &);
  auxilia::Status delete_watchpoint(size_t);
  void update_watchpoints(bool,bool);

private:
  WatchPoints watchpoints_;
  expression::Visitor* visitor_;
};

} // namespace accat::luce::repl
