#include "Color.h"
#include "Script/LuaFunc.h"

namespace app {
namespace script {

static const s8* const GMETA_NAME_COLOR = "Color";
#if defined(DUSE_SCRIPT_NAMESPACE)
static const s8* const GMETA_NAMESPACE_COLOR = "core";
static const s8* const GMETA_FULL_NAME_COLOR = "core_Color"; // idx of reg_table
#else
static const s8* const GMETA_NAMESPACE_COLOR = nullptr;
static const s8* const GMETA_FULL_NAME_COLOR = GMETA_NAME_COLOR; // idx of reg_table
#endif

/**
 * @brief  鏋勯€犲嚱鏁