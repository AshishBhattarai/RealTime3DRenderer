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
  ComponentArray() {
    int reserveSize = MAX_ENTITES / 4;
    componentArray.reserve(reserveSize);
    entityToIndexMap.reserve(reserveSize);
    indexToEntityMap.reserve(reserveSize);
  }

  /**
   * @brief insertData
   * @param entity
   * @param component
   *
   * Doesn't check whether the given entity already has the component or not
   */
  void insertData(Entity entity, T component) {
    size_t newIndex = componentArray.size();
    entityToIndexMap[entity] = newIndex;
    indexToEntityMap[newIndex] = entity;
    componentArray.push_back(component);
  }

  /**
   * @brief removeData
   * @param entity
   *
   * Doesn't check whether the given entity has the component or not
   */
  void removeData(Entity entity) {
    assert(!componentArray.empty());
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

  /**
   * @brief GetData
   * @param entity
   * @return
   *
   * Doesn't check whether the given entity has the component or not
   */
  T &getData(Entity entity) {
    assert(!componentArray.empty());
    return componentArray[entityToIndexMap[entity]];
  }

  /**
   * @brief EntityDestoryed
   *
   * Doesn't check whether the given entity has the component or not
   */
  void entityDestoryed(Entity entity) override { removeData(entity); }

  size_t getSize() const { return componentArray.size(); }

private:
  std::vector<T> componentArray;
  // entity id to componentArrayIndex map
  std::unordered_map<Entity, size_t> entityToIndexMap;
  // entity componentArrayIndex to entity id map
  std::unordered_map<size_t, Entity> indexToEntityMap;
};
} // namespace ecs
