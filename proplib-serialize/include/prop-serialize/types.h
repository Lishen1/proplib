#pragma once
/*!
 * \file types.h
 *
 * \author Пантелюк П.А
 * \date Март 2018
 *
 * 
 */
#include <cstdint>
namespace proplib
{
  enum class res_t
  {
    ok = 0,
    key_not_found,
    all_skiped,
    not_all_deser,
    error,
  };

  struct stat_t
  {
    size_t key_not_found_cnt = 0;
    size_t all_skiped_cnt = 0;
    size_t not_all_deser_cnt = 0;
  };
}