#ifndef __COMMON_H__
#define __COMMON_H__

namespace daemon_manager {

#define RETURN_IF(cond, value)                                                 \
  ({                                                                           \
    if ((cond)) {                                                              \
      return (value);                                                          \
    }                                                                          \
  })

#define RETURN_IF_FAIL(cond, value) RETURN_IF(!(cond), value)

} // namespace daemon_manager

#endif /* __COMMON_H__ */
