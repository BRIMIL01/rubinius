#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"

#include <iostream>

namespace rubinius {
  void CompactLookupTable::init(STATE) {
    GO(compactlookuptable).set(state->new_class("CompactLookupTable",
          G(tuple), CompactLookupTable::fields));
    G(compactlookuptable)->set_object_type(state, CompactLookupTableType);
  }

  CompactLookupTable* CompactLookupTable::create(STATE) {
    return state->om->new_object_variable<CompactLookupTable>
      (G(compactlookuptable), COMPACTLOOKUPTABLE_SIZE);
  }

  Object* CompactLookupTable::fetch(STATE, Object* key) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(state, i) == key) return at(state, i + 1);
    }

    return Qnil;
  }

  Object* CompactLookupTable::store(STATE, Object* key, Object* val) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* tmp = at(state, i);
      if(tmp == key || tmp->nil_p()) {
        put(state, i, key);
        put(state, i + 1, val);
        return Qtrue;
      }
    }

    return Qfalse;
  }

  Object* CompactLookupTable::has_key(STATE, Object* key) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(state, i) == key) return Qtrue;
    }

    return Qfalse;
  }

  Array* CompactLookupTable::keys(STATE) {
    Array* ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p()) ary->append(state, key);
    }

    return ary;
  }

  Array* CompactLookupTable::values(STATE) {
    Array* ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p()) ary->append(state, at(state, i + 1));
    }

    return ary;
  }

  LookupTable* CompactLookupTable::to_lookuptable(STATE) {
    LookupTable* tbl = (LookupTable*)LookupTable::create(state);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p()) tbl->store(state, key, at(state, i + 1));
    }

    return tbl;
  }

  void CompactLookupTable::Info::show(STATE, Object* self, int level) {
    CompactLookupTable* tbl = as<CompactLookupTable>(self);
    Array* keys = tbl->keys(state);
    size_t size = keys->size();

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(size_t i = 0; i < size; i++) {
      std::cout << ":" << as<Symbol>(keys->get(state, i))->c_str(state);
      if(i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }
}
