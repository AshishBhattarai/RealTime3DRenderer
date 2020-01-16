#include "common.h"
#include "third_party/simplesignal.h"
#include <memory>

namespace ecs {
using EventSignal = Simple::Signal<void(const void *)>;
using EventSignalPtr = std::shared_ptr<EventSignal>;
using EventSignalWeakPtr = std::weak_ptr<EventSignal>;

class BaseEvent {
protected:
  static EventFamily familyCount;
};

template <typename E> class Event : public BaseEvent {
public:
  static EventFamily getFamily() { return family; }

private:
  static EventFamily family;
  friend class EventManager;

  static EventFamily genFamily() {
    if (!family)
      family = familyCount++;
    return family;
  }
  friend class EventManager;
};

/**
 * Base class for all receivers.
 *
 * All receivers must have a receiver method.
 *
 * E : Must be a valid registered event.
 */
template <typename E> class Receiver {
public:
  Receiver() {
    assert(Event<E>::getFamily() && "typename E must be a registered event.");
  }
  virtual ~Receiver() {
    // disconnect receiver
    if (!connection.first.expired())
      connection.first.lock()->disconnect(connection.second);
  }

private:
  // signal ptr and connection id
  std::pair<EventSignalWeakPtr, size_t> connection;
  friend class EventManager;
};

class EventManager : NonCopyable {
public:
  template <typename E> void registerEvent() {
    assert(!Event<E>::family && "Event can be registered only once.");
    EventFamily family = Event<E>::genFamily();
    if (family >= handlers.size())
      handlers.resize(handlers.size() + 1);
    handlers[family] = std::make_shared<EventSignal>();
  }

  // subscribe to an event E with receiver R
  template <typename E, typename R> void subscribe(Receiver<E> &receiver) {
    assert(!Event<E>::family && "Event must be registered before use." &&
           !receiver.connection.first.expired() &&
           "Receiver is already subscribed to an event.");
    // Receiver must have ::receive method
    void (R::*receive)(const E &) = &R::receive;
    auto wrapper = EventCallbackWrapper<E>(
        std::bind(receive, &receiver, std::placeholders ::_1));
    auto signal = handlers[Event<E>::family];
    auto connectionId = signal->connect(wrapper);
    receiver.connection =
        std::make_pair(EventSignalWeakPtr(signal), connectionId);
    receiver.connectedSignalsCount++;
  }

  // unsubscribe
  template <typename E, typename R> void unsubscribe(Receiver<E> &receiver) {
    assert(!Event<E>::family && "Event must be registered before use." &&
           !receiver.connection.first.expired() &&
           "Receiver is already subscribed to an event.");
    auto &ptr = receiver.connection.first;
    if (!ptr.expired())
      ptr.lock()->disconnect(receiver.connection.second);
  }

  // emit events
  template <typename E> void emit(const E &event) {
    assert(!Event<E>::family && "Event must be registered before use.");
    handlers[Event<E>::family]->emit(&event);
  }

  // construct a new object of type E and emit
  template <typename E, typename... Args> void emit(Args &&... args) {
    assert(!Event<E>::family && "Event must be registered before use.");
    E event(std::forward(args)...); // unfold args with std::forward
    handlers[Event<E>::family]->emit(&event);
  }

  size_t totalConnectedReceivers() const {
    size_t size = 0;
    for (EventSignalPtr handler : handlers) {
      if (handler)
        size += handler->size();
    }
    return size;
  }

private:
  template <typename E> constexpr void checkIsDerived() {
    static_assert(std::is_base_of<Event<E>, E>::value,
                  "T not derived from System.");
  }

  /**
   * Functor used as an event signal callback that casts the callback
   * parameter (const void *) to Event E
   */
  template <typename E> struct EventCallbackWrapper {
    explicit EventCallbackWrapper(std::function<void(const E &)> callback)
        : callback(callback) {}

    /**
     * Cast signal callback param (const void*) to Event(E) and pass it to
     * receiver callback(Receiver::receive).
     */
    void operator()(const void *event) {
      callback(*(static_cast<const E *>(event)));
    }

    std::function<void(const E &)> callback;
  };

  std::vector<EventSignalPtr> handlers;
};

} // namespace ecs
