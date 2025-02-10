// test for mediator pattern

// mediator pattern hpp
#include <iostream>
struct IMediator;
struct IComponent;

struct IMediator {
  virtual void notify(IComponent *sender,
                      int senderEnum,
                      int targetEnum,
                      int targetEvent) = 0;
};

struct IComponent {
protected:
  IMediator *mediator;

public:
  IComponent(IMediator *mediator) : mediator(mediator) {}

  virtual void send(int target, int targetEvent) = 0;
  virtual void receive(int action, IComponent *sender, int senderEnum) = 0;
};

// common hpp
enum /* class */ [[clang::enum_extensibility(closed)]] ComponentEnum {
  kComponent1 = 1,
  kComponent2,
  kComponent3
};

// component1 hpp
struct Component2;
struct Component3;
struct Component1 : IComponent {
  enum /* class */ [[clang::enum_extensibility(closed)]] Action {
    kEvent1 = 1,
    kEvent2,
    kEvent3
  };
  Component1(IMediator *mediator) : IComponent(mediator) {}

  void send(int target, int targetEvent) override {
    std::cout << "Component1 sent Event" << targetEvent << " to Component"
              << target << std::endl;
    mediator->notify(this, (ComponentEnum::kComponent1), target, targetEvent);
  }
  void receive(int action,
               [[maybe_unused]] IComponent *sender,
               int senderEnum) override {
    switch (static_cast<Action>(action)) {
    case Action::kEvent1:
      std::cout << "Component1 received Event1 from Component" << senderEnum
                << std::endl;
      break;
    case Action::kEvent2:
      std::cout << "Component1 received Event2 from Component" << senderEnum
                << std::endl;
      break;
    case Action::kEvent3:
      std::cout << "Component1 received Event3 from Component" << senderEnum
                << std::endl;
      break;
    }
    // if we want to use Sender, than move the func definition to cpp
  }
};
// component2 hpp
struct Component1;
struct Component3;
struct Component2 : IComponent {
  enum /* class */ [[clang::enum_extensibility(closed)]] Action {
    kEvent1 = 1,
    kEvent2,
    kEvent3
  };
  Component2(IMediator *mediator) : IComponent(mediator) {}

  void send(int target, int targetEvent) override {
    std::cout << "Component2 sent Event" << targetEvent << " to Component"
              << target << std::endl;
    mediator->notify(this, (ComponentEnum::kComponent2), target, targetEvent);
  }
  void receive(int, IComponent *, int senderEnum) override {
    // ditto
    std::cout << "Component2 received Event" << senderEnum << " from Component"
              << senderEnum << std::endl;
  }
};
// component3 hpp
// ...

// monitor hpp
// include component1,2,3 hpp
struct Monitor;

struct Monitor : IMediator {
  Component1 component1;
  Component2 component2;
  // Component3 component3;
  Monitor() : component1(this), component2(this) {}

  void notify(IComponent *sender,
              int senderEnum,
              int targetEnum,
              int targetEvent) override {
    switch (static_cast<ComponentEnum>(targetEnum)) {
    case ComponentEnum::kComponent1:
      std::cout << "Monitor received Event from Component" << senderEnum
                << " and sent to Component1" << std::endl;
      component1.receive(targetEvent, sender, senderEnum);
      break;
    case ComponentEnum::kComponent2:
      std::cout << "Monitor received Event from Component" << senderEnum
                << " and sent to Component2" << std::endl;
      component2.receive(targetEvent, sender, senderEnum);
      break;
    default:
      break;
    }
  }
};

int main() {
  Monitor monitor;
  monitor.component1.send((ComponentEnum::kComponent2),
                          (Component1::Action::kEvent1));
  monitor.component2.send((ComponentEnum::kComponent1),
                          (Component2::Action::kEvent1));
  return 0;
}
