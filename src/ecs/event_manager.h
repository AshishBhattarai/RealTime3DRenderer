#pragma once

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
  static EventFamily family; // EventFamily is int
  friend class EventManager;

  static EventFamily genFamily() {
    if (!family)
      family = familyCount++;
    return family;
  }
  friend class EventManager;
};
template <typename T> EventFamily Event<T>::family = INVALID_EVENT_FAMILY;

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
  static EventManager &getInstace() {
    static EventManager instance;
    return instance;
  }

  template <typename E> void registerEvent() {
    checkIsDerived<E>();
    assert(!Event<E>::family && "Event can be registered only once.");
    EventFamily family = Event<E>::genFamily();
    if (family >= handlers.size())
      handlers.resize(handlers.size() + 1);
    // since valid event component family starts from 1 and arrays start from 0
    handlers[family - 1] = std::make_shared<EventSignal>();
  }

  // subscribe to an event E with receiver R
  template <typename E, typename R> void subscribe(R &receiver) {
    assert(Event<E>::family && "Event must be registered before use." &&
           receiver.connection.first.expired() &&
           "Receiver is already subscribed to an event.");
    // Receiver must have ::receive method
    void (R::*receive)(const E &) = &R::receive;
    auto wrapper = EventCallbackWrapper<E>(
        std::bind(receive, &receiver, std::placeholders::_1));
    auto signal = handlers[Event<E>::family - 1];
    auto connectionId = signal->connect(wrapper);
    receiver.connection =
        std::make_pair(EventSignalWeakPtr(signal), connectionId);
  }

  // unsubscribe
  template <typename E, typename R> void unsubscribe(R &receiver) {
    assert(Event<E>::family && "Event must be registered before use." &&
           !receiver.connection.first.expired() &&
           "Receiver is not subscribed to ant event.");
    auto &ptr = receiver.connection.first;
    if (!ptr.expired())
      ptr.lock()->disconnect(receiver.connection.second);
  }

  // emit events
  template <typename E> void emit(const E &event) {
    assert(Event<E>::family && "Event must be registered before use.");
    handlers[Event<E>::family - 1]->emit(&event);
  }

  // construct a new object of type E and emit
  template <typename E, typename... Args> void emit(Args &&... args) {
    assert(Event<E>::family && "Event must be registered before use.");
    E event(std::forward<Args>(args)...); // unfold args with std::forward
    handlers[Event<E>::family - 1]->emit(&event);
  }

  size_t totalConnectedReceivers() const {
    size_t size = 0;
    for (EventSignalPtr handler : handlers) {
      if (handler)
        size += handler->size();
    }
    return size;
  }

  size_t totalEvents() const { return handlers.size(); }

private:
  EventManager() = default;
  ~EventManager() = default;

  template <typename E> constexpr void checkIsDerived() {
    static_assert(std::is_base_of<Event<E>, E>::value,
                  "T not derived from Event.");
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
