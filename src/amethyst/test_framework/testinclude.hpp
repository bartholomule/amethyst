/*
 * $Id: testinclude.hpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
 *
 * Part of "Amethyst Palace" - A playground for future graphics ideas
 * Copyright (C) 2003 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(__TESTINCLUDE_HPP__)
#define            __TESTINCLUDE_HPP__

#include <iostream>

/**
 * This file is only for use in testing.  It has no place in actual running
 * program.
 */

#define TEST_EXCEPTION_TEST(try_block, catch_block) \
   do                                               \
   {                                                \
      try                                           \
      {                                             \
         try_block;                                 \
      }                                             \
      catch(...)                                    \
      {                                             \
         catch_block;                               \
      }                                             \
   }                                                \
   while(0)

#define TEST_EXCEPTION_CATCH(try_block, catch_def, catch_block) \
   do                                                           \
   {                                                            \
      try                                                       \
      {                                                         \
         try_block;                                             \
      }                                                         \
      catch(catch_def)                                          \
      {                                                         \
         catch_block;                                           \
      }                                                         \
   }                                                            \
   while(0)

#define TEST_EXCEPTION_NAMED(name, try_block, not_thrown, thrown) \
   do                                                             \
   {                                                              \
      std::cout << "Trying " << name << "... ";                   \
      try                                                         \
      {                                                           \
         try_block;                                               \
         std::cout << not_thrown << std::endl;                    \
      }                                                           \
      catch(...)                                                  \
      {                                                           \
         std::cout << thrown << std::endl;                        \
      }                                                           \
   }                                                              \
   while(0)

#define TEST_RESULT_NAMED(name, statement, if_true, if_false) \
   do                                                         \
   {                                                          \
      std::cout << "Checking " << name << "... ";             \
      if( statement )                                         \
      {                                                       \
         std::cout << if_true << std::endl;                   \
      }                                                       \
      else                                                    \
      {                                                       \
         std::cout << if_false << std::endl;                  \
      }                                                       \
   }                                                          \
   while(0)

#endif /* !defined(__TESTINCLUDE_HPP__) */
