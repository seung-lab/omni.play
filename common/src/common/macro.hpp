#pragma once

#define PROP(type, name)                             \
 protected:                                          \
  type name##_;                                      \
                                                     \
 public:                                             \
  inline type name() { return name##_; }             \
  inline const type name() const { return name##_; } \
                                                     \
 protected:

#define PROP_CONST(type, name)                       \
 protected:                                          \
  const type name##_;                                \
                                                     \
 public:                                             \
  inline const type name() const { return name##_; } \
                                                     \
 protected:

#define PROP_SET(type, name)                          \
 protected:                                           \
  type name##_;                                       \
                                                      \
 public:                                              \
  inline type name() { return name##_; }              \
  inline const type name() const { return name##_; }  \
  inline void set_##name(type val) { name##_ = val; } \
                                                      \
 protected:

#define PROP_REF(type, name)                          \
 protected:                                           \
  type name##_;                                       \
                                                      \
 public:                                              \
  inline type& name() { return name##_; }             \
  inline const type& name() const { return name##_; } \
                                                      \
 protected:

#define PROP_REF_SET(type, name)                      \
 protected:                                           \
  type name##_;                                       \
                                                      \
 public:                                              \
  inline type& name() { return name##_; }             \
  inline const type& name() const { return name##_; } \
  inline void set_##name(type val) { name##_ = val; } \
                                                      \
 protected:

#define PROP_CONST_REF(type, name)                    \
 protected:                                           \
  type name##_;                                       \
                                                      \
 public:                                              \
  inline const type& name() const { return name##_; } \
                                                      \
 protected:

#define PROP_CONST_PTR(type, name)                    \
 protected:                                           \
  const type* name##_;                                \
                                                      \
 public:                                              \
  inline const type* name() const { return name##_; } \
                                                      \
 protected:

#define GET_VAR_PROP(objectType, dataType, name)                 \
  class name##_getter : public boost::static_visitor<dataType> { \
   public:                                                       \
    template <typename T>                                        \
    dataType operator()(const objectType<T>& obj) const {        \
      return obj.name();                                         \
    }                                                            \
  };                                                             \
  inline dataType get_##name(const objectType##Var& obj) {       \
    return boost::apply_visitor(name##_getter(), obj);           \
  }
