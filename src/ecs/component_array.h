#pragma once

#include "common.h"
#include <unordered_map>
#include <vector>

namespace ecs {

/**
 * @brief The BaseComponentArray class
 * Parent class for ComponentArray to destory them polymorphically without
 * knowing their generic type.
 *
 * To destory all the components of an entity.
 */
class BaseComponentArray : NonCopyable {
public:
  virtual ~BaseComponentArray() = default;
  virtual void entityDestoryed(Entity entity) = 0;
};

/**
 * @brief The ComponentArray class
 * Packed array that stores a collection components of type T that exists.
 */
template <typename T> class ComponentArray : public BaseComponentArray {
public:
  ComponentArray() { reserve(); }

  /**
   * @brief insertData
   * @param entity
   * @param component
   * @return true if vector reallocation(component cache invalid)
   */
  bool insertData(Entity entity, T component) {
    assert(entityToIndexMap.find(entity) == entityToIndexMap.end() &&
           "Component added to same entity more than once.");
    size_t newIndex = componentArray.size();
    entityToIndexMap[entity] = newIndex;
    indexToEntityMap[newIndex] = entity;
    componentArray.push_back(component);
    if (newIndex >= reserveSize) {
      reserveSize += RESERVE_BLOCK;
      reserve();
      return true;
    } else {
      return false;
    }
  }

  void removeData(Entity entity) {
    assert(entityToIndexMap.find(entity) != entityToIndexMap.end() &&
           "Removing non-existent component.");
    // move remove entity to the end componentArray
    size_t removeIndex = entityToIndexMap[entity];
    size_t lastIndex = componentArray.size() - 1;
    Entity lastIndexEntity = indexToEntityMap[lastIndex];
    componentArray[removeIndex] = componentArray[lastIndex];

    // pop back and update maps
    componentArray.pop_back();
    entityToIndexMap[lastIndexEntity] = removeIndex;
    indexToEntityMap[removeIndex] = lastIndexEntity;
    entityToIndexMap.erase(entity);
    indexToEntityMap.erase(lastIndex);
  }

  T &getData(Entity entity) {
    assert(entityToIndexMap.find(entity) != entityToIndexMap.end() &&
           "Retrieving non-existent component.");
    return componentArray[entityToIndexMap[entity]];
  }

  /**
   * @brief EntityDestoryed
   * Doesn't check whether the given entity has the component or not
   */
  void entityDestoryed(Entity entity) override {
    assert(entityToIndexMap.find(entity) != entityToIndexMap.end());
    removeData(entity);
  }

  size_t getSize() const { return componentArray.size(); }

private:
  static constexpr size_t RESERVE_BLOCK = MAX_ENTITES / 4;
  size_t reserveSize = RESERVE_BLOCK;
  std::vector<T> componentArray;
  // entity id to componentArrayIndex map
  std::unordered_map<Entity, size_t> entityToIndexMap;
  // entity componentArrayIndex to entity id map
  std::unordered_map<size_t, Entity> indexToEntityMap;

  void reserve() {
    componentArray.reserve(reserveSize);
    entityToIndexMap.reserve(reserveSize);
    indexToEntityMap.reserve(reserveSize);
  }
};
} // namespace ecs
