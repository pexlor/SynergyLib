#include "thread.h"

namespace Pliber
{
static thread_local Thread * t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKONW";


}