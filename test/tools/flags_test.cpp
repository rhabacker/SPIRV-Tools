// Copyright (c) 2023 Google LLC.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tools/util/flags.h"

#include "gmock/gmock.h"

#ifdef UTIL_FLAGS_FLAG
#undef UTIL_FLAGS_FLAG
#define UTIL_FLAGS_FLAG(Type, Prefix, Name, Default, Required, IsShort)     \
  flags::Flag<Type> Name(Default);                                          \
  flags::FlagRegistration Name##_registration(Name, Prefix #Name, Required, \
                                              IsShort)
#else
#error \
    "UTIL_FLAGS_FLAG is not defined. Either flags.h is not included of the flag name changed."
#endif

class FlagTest : public ::testing::Test {
 protected:
  void SetUp() override { flags::FlagList::reset(); }
};

TEST_F(FlagTest, NoFlags) {
  const char* argv[] = {"binary", nullptr};
  EXPECT_TRUE(flags::Parse(argv));
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, DashIsPositional) {
  const char* argv[] = {"binary", "-", nullptr};

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(flags::positional_arguments.size(), 1);
  EXPECT_EQ(flags::positional_arguments[0], "-");
}

TEST_F(FlagTest, Positional) {
  const char* argv[] = {"binary", "A", "BCD", nullptr};

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(flags::positional_arguments.size(), 2);
  EXPECT_EQ(flags::positional_arguments[0], "A");
  EXPECT_EQ(flags::positional_arguments[1], "BCD");
}

TEST_F(FlagTest, MissingRequired) {
  FLAG_SHORT_bool(g, false, true);

  const char* argv[] = {"binary", nullptr};
  EXPECT_FALSE(flags::Parse(argv));
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, BooleanShortValue) {
  FLAG_SHORT_bool(g, false, false);
  const char* argv[] = {"binary", "-g", nullptr};
  EXPECT_FALSE(g.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(g.value());
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, BooleanShortDefaultValue) {
  FLAG_SHORT_bool(g, true, false);
  const char* argv[] = {"binary", nullptr};
  EXPECT_TRUE(g.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(g.value());
}

TEST_F(FlagTest, BooleanLongValueNotParsed) {
  FLAG_SHORT_bool(g, false, false);
  const char* argv[] = {"binary", "-g", "false", nullptr};
  EXPECT_FALSE(g.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(g.value());
  EXPECT_EQ(flags::positional_arguments.size(), 1);
  EXPECT_EQ(flags::positional_arguments[0], "false");
}

TEST_F(FlagTest, BooleanLongSplitNotParsed) {
  FLAG_LONG_bool(foo, false, false);
  const char* argv[] = {"binary", "--foo", "true", nullptr};
  EXPECT_FALSE(foo.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(foo.value());
  EXPECT_EQ(flags::positional_arguments.size(), 1);
  EXPECT_EQ(flags::positional_arguments[0], "true");
}

TEST_F(FlagTest, BooleanLongExplicitTrue) {
  FLAG_LONG_bool(foo, false, false);
  const char* argv[] = {"binary", "--foo=true", nullptr};
  EXPECT_FALSE(foo.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(foo.value());
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, BooleanLongExplicitFalse) {
  FLAG_LONG_bool(foo, false, false);
  const char* argv[] = {"binary", "--foo=false", nullptr};
  EXPECT_FALSE(foo.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_FALSE(foo.value());
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, BooleanLongDefaultValue) {
  FLAG_LONG_bool(foo, true, false);
  const char* argv[] = {"binary", nullptr};
  EXPECT_TRUE(foo.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_TRUE(foo.value());
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, BooleanLongDefaultValueCancelled) {
  FLAG_LONG_bool(foo, true, false);
  const char* argv[] = {"binary", "--foo=false", nullptr};
  EXPECT_TRUE(foo.value());

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_FALSE(foo.value());
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringFlagDefaultValue) {
  FLAG_SHORT_string(f, "default", false);
  const char* argv[] = {"binary", nullptr};
  EXPECT_EQ(f.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));
  EXPECT_EQ(f.value(), "default");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringFlagShortMissingString) {
  FLAG_SHORT_string(f, "default", false);
  const char* argv[] = {"binary", "-f", nullptr};
  EXPECT_EQ(f.value(), "default");

  EXPECT_FALSE(flags::Parse(argv));
}

TEST_F(FlagTest, StringFlagDefault) {
  FLAG_SHORT_string(f, "default", false);
  const char* argv[] = {"binary", nullptr};
  EXPECT_EQ(f.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(f.value(), "default");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringFlagSet) {
  FLAG_SHORT_string(f, "default", false);
  const char* argv[] = {"binary", "-f", "toto", nullptr};
  EXPECT_EQ(f.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(f.value(), "toto");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringLongFlagSetSplit) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--foo", "toto", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(foo.value(), "toto");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringLongFlagSetUnified) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--foo=toto", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(foo.value(), "toto");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, StringLongFlagSetEmpty) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--foo=", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(foo.value(), "");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, AllPositionalAfterDoubleDash) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--", "--foo=toto", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(foo.value(), "default");
  EXPECT_EQ(flags::positional_arguments.size(), 1);
  EXPECT_EQ(flags::positional_arguments[0], "--foo=toto");
}

TEST_F(FlagTest, NothingAfterDoubleDash) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_TRUE(flags::Parse(argv));

  EXPECT_EQ(foo.value(), "default");
  EXPECT_EQ(flags::positional_arguments.size(), 0);
}

TEST_F(FlagTest, FlagDoubleSetNotAllowed) {
  FLAG_LONG_string(foo, "default", false);
  const char* argv[] = {"binary", "--foo=abc", "--foo=def", nullptr};
  EXPECT_EQ(foo.value(), "default");

  EXPECT_FALSE(flags::Parse(argv));
}

TEST_F(FlagTest, MultipleFlags) {
  FLAG_LONG_string(foo, "default foo", false);
  FLAG_LONG_string(bar, "default_bar", false);
  const char* argv[] = {"binary", "--foo", "abc", "--bar=def", nullptr};
  EXPECT_EQ(foo.value(), "default foo");
  EXPECT_EQ(bar.value(), "default_bar");

  EXPECT_TRUE(flags::Parse(argv));
  EXPECT_EQ(foo.value(), "abc");
  EXPECT_EQ(bar.value(), "def");
}

TEST_F(FlagTest, MixedStringAndBool) {
  FLAG_LONG_string(foo, "default foo", false);
  FLAG_LONG_string(bar, "default_bar", false);
  FLAG_SHORT_bool(g, false, false);
  const char* argv[] = {"binary", "--foo", "abc", "-g", "--bar=def", nullptr};
  EXPECT_EQ(foo.value(), "default foo");
  EXPECT_EQ(bar.value(), "default_bar");
  EXPECT_FALSE(g.value());

  EXPECT_TRUE(flags::Parse(argv));
  EXPECT_EQ(foo.value(), "abc");
  EXPECT_EQ(bar.value(), "def");
  EXPECT_TRUE(g.value());
}
