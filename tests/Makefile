#this is meant to be run from PROJECT_ROOT/SUBPROJECT/lib/mdrivlib

DOCTEST_DIR ?= ../../../shared/doctest/doctest
TEST_DIR ?= tests

TEST_SOURCES = $(TEST_DIR)/doctest.cc
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_test.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_test.cpp)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_tests.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_tests.cpp)
TEST_SOURCES += $(wildcard $(TEST_DIR)/test_*.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/test_*.cpp)
TEST_SOURCES += $(TEST_DIR)/stubs/pin.cc
TEST_SOURCES += $(TEST_DIR)/stubs/stm32/fake_gpio.cpp
TEST_SOURCES += drivers/rotary.cc


BUILDDIR = build

.PHONY: tests clean FORCE

CXXFLAGS = 	-Wall \
		 	-std=gnu++2a \
			-stdlib=libc++ \
			-include tests/stubs/stm32/fake_gpio.h \
			-I$(DOCTEST_DIR) \
			-I$(TEST_DIR) \
			-I$(TEST_DIR)/stubs \
			-I$(TEST_DIR)/stubs/stm32 \
			-I. \
			-I../stm32/cmsis/include \
			-I../stm32/device/include \
			-I../stm32/periph/include \
			-DSTM32H7 \
			-DSTM32H755xx \
			-DCORE_CM7 \
			-DTESTPROJECT \
			-Wno-deprecated-volatile \

tests: $(BUILDDIR)/runtests
	$(BUILDDIR)/runtests

#Todo: build test sources individually into .o files, and use -MM -MF -MMD to check dependencies
$(BUILDDIR)/runtests: $(TEST_SOURCES) FORCE
	@mkdir -p $(@D)
	clang++ $(CXXFLAGS) $(TEST_SOURCES) -o $(BUILDDIR)/runtests

clean:
	rm -rf $(BUILDDIR)

