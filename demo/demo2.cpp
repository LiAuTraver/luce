#include <argparse/argparse.hpp>

// Example showing different argument types
int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("demo");

  // Boolean flag example
  program.add_argument("--verbose")
      .default_value(false)  // When not specified: false
      .implicit_value(true); // When just --verbose: true

  // Counter example
  program.add_argument("--count")
      .default_value(0)  // When not specified: 0
      .implicit_value(1) // When just --count: 1
      .nargs(0);         // Takes no arguments

  // Value with required argument
  program.add_argument("--name")
      .default_value(std::string("unknown")) // When not specified: "unknown"
      .required();                           // Must be provided

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  // Usage
  bool verbose = program.get<bool>("--verbose");
  int count = program.get<int>("--count");
  std::string name = program.get<std::string>("--name");
  std::cout << "Verbose: " << verbose << std::endl;
  std::cout << "Count: " << count << std::endl;
  std::cout << "Name: " << name << std::endl;

  return 0;
}

/*
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <unordered_set>

class Argument {
public:
  enum class Type : uint8_t {
    kFlag,      // Boolean flag, e.g., --verbose
    kSingle,    // Argument with a single value, e.g., --output file.txt
    kMulti,     // Argument with multiple values, e.g., --files file1 file2
    kPositional // Positional argument, e.g., filename
  };

private:
  std::string name_;              // Long name of the argument (e.g., --file)
  std::optional<char> shortName_; // Short name of the argument (e.g., -f)
  Type type_ = Type::kFlag;       // Type of the argument
  std::string description_;       // Description for help text
  std::optional<std::string> defaultValue_; // Default value (if any)
  std::vector<std::string> values_;         // Parsed values for the argument
  std::unordered_set<std::string>
      conflicts_; // Arguments that conflict with this one
  std::unordered_set<std::string>
      dependencies_; // Arguments required for this one
  std::function<bool(const std::vector<std::string> &)>
      validator_; // Custom validation function

  bool isPresent_ =
      false; // Indicates whether the argument was found in the input

public:
  // Constructor
  Argument(const std::string &name,
           Type type,
           const std::string &description = "")
      : name_(name), type_(type), description_(description) {}

  // Setters for optional fields
  Argument &setShortName(char shortName) {
    shortName_ = shortName;
    return *this;
  }

  Argument &setDefaultValue(const std::string &defaultValue) {
    defaultValue_ = defaultValue;
    return *this;
  }

  Argument &addConflict(const std::string &conflictingArg) {
    conflicts_.insert(conflictingArg);
    return *this;
  }

  Argument &addDependency(const std::string &dependentArg) {
    dependencies_.insert(dependentArg);
    return *this;
  }

  Argument &setValidator(
      const std::function<bool(const std::vector<std::string> &)> &validator) {
    validator_ = validator;
    return *this;
  }

  // Methods to process and validate arguments
  void markPresent() { isPresent_ = true; }

  void addValue(const std::string &value) { values_.push_back(value); }

  bool validate() const {
    if (validator_) {
      return validator_(values_);
    }
    return true; // Default to true if no custom validator is provided
  }

  // Accessors
  const std::string &getName() const { return name_; }
  std::optional<char> getShortName() const { return shortName_; }
  Type getType() const { return type_; }
  const std::string &getDescription() const { return description_; }
  const std::optional<std::string> &getDefaultValue() const {
    return defaultValue_;
  }
  const std::vector<std::string> &getValues() const { return values_; }
  bool isPresent() const { return isPresent_; }
  const std::unordered_set<std::string> &getConflicts() const {
    return conflicts_;
  }
  const std::unordered_set<std::string> &getDependencies() const {
    return dependencies_;
  }

  // Utility for help text
  std::string toHelpString() const {
    std::string help = name_;
    if (shortName_)
      help += ", -" + std::string(1, *shortName_);
    help += ": " + description_;
    if (defaultValue_)
      help += " (default: " + *defaultValue_ + ")";
    return help;
  }
};

int main(int argc, char *argv[]) {

  std::ranges::copy(
      argv, argv + argc, std::ostream_iterator<char *>(std::cout, "\n"));

  Argument outputArg(
      "--output", Argument::Type::kSingle, "Specifies the output file");
  outputArg.setShortName('o')
      .setDefaultValue("output.txt")
      .addConflict("--verbose")
      .setValidator([](const std::vector<std::string> &values) {
        return !values.empty() && values[0].find(".txt") != std::string::npos;
      });

  Argument verboseArg(
      "--verbose", Argument::Type::kFlag, "Enables verbose output");
  verboseArg.setShortName('v');

  std::cout << outputArg.toHelpString() << std::endl;
  std::cout << verboseArg.toHelpString() << std::endl;
}
*/
