#pragma once

#define PROP_REF(type, name)           \
private:                               \
	type name##_;                      \
public:                                \
	inline type & name() {             \
		return name##_;                \
	}                                  \
	inline const type & name() const { \
		return name##_;                \
	}

#define PROP_REF_SET(type, name)       \
private:                               \
	type name##_;                      \
public:                                \
	inline type & name() {             \
		return name##_;                \
	}                                  \
	inline const type & name() const { \
		return name##_;                \
	}                                  \
	inline void set_##name(type val) { \
		name##_ = val;                 \
	}

#define PROP_CONST_REF(type, name)     \
private:                               \
	type name##_;                      \
public:                                \
	inline const type & name() const { \
		return name##_;                \
	}
