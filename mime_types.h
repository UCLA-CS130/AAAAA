#ifndef MIME_TYPES_DEFINED
#define MIME_TYPES_DEFINED

#include <string>

namespace mime_types {

/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string& extension);

} // namespace mime_types

#endif
