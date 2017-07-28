//===--- BlockReadableOrder.cpp - BlockReadableOrderVisitor impl ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/BlockReadableOrder.h"

namespace clang {
namespace spirv {

void BlockReadableOrderVisitor::visit(BasicBlock *block) {
  if (doneBlocks.count(block) || todoBlocks.count(block))
    return;

  callback(block);

  doneBlocks.insert(block);

  // Check the continue and merge targets. If any one of them exists, we need
  // to make sure visiting it is delayed until we've done the rest.

  BasicBlock *continueBlock = block->getContinueTarget();
  BasicBlock *mergeBlock = block->getMergeTarget();

  if (continueBlock)
    todoBlocks.insert(continueBlock);

  if (mergeBlock)
    todoBlocks.insert(mergeBlock);

  for (BasicBlock *successor : block->getSuccessors())
    visit(successor);

  // Handle continue and merge targets now.

  if (continueBlock) {
    todoBlocks.erase(continueBlock);
    visit(continueBlock);
  }

  if (mergeBlock) {
    todoBlocks.erase(mergeBlock);
    visit(mergeBlock);
  }
}

} // end namespace spirv
} // end namespace clang
