// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ASTConsumer.h"

//-----------------------------------------------------------------------------
// FrontendAction for Reflector Plugin
//-----------------------------------------------------------------------------
class ReflectorAction: public clang::PluginASTAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI, StringRef InFile) override {
    // Module* m = CI.getPreprocessor().getHeaderSearchInfo().lookupModule("fabric.refl");
    // if (m && m->isAvailable() && !m->isUnimportable()) {
    //     llvm::outs() << "[ERROR] MODULE ACCESSIBLE " << m->getPrimaryModuleInterfaceName() <<
    //     "\n";
    // }
    return std::unique_ptr<clang::ASTConsumer>(
      std::make_unique<ReflectorASTConsumer>(&CI, &CI.getASTContext())
    );
  }


  bool ParseArgs(const CompilerInstance& CI, const std::vector<std::string>& args) override {
    return true;
  }

  ActionType getActionType() override {
    return ActionType::AddBeforeMainAction;
  }
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<ReflectorAction> X("reflector", "Adds static type information to structs and classes.");

namespace {
  struct ExampleAttrInfo: public ParsedAttrInfo {
    ExampleAttrInfo() {
      OptArgs                       = 1;
      static constexpr Spelling S[] = {
        {ParsedAttr::AS_GNU, "ignore"},
        {ParsedAttr::AS_C23, "ignore"},
        {ParsedAttr::AS_CXX11, "ignore"},
        {ParsedAttr::AS_CXX11, "refl::ignore"}
      };
      Spellings = S;
    }

    bool diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const override {
      // This attribute appertains to fields only.
      if (!isa<FieldDecl>(D)) {
        S.Diag(Attr.getLoc(), diag::warn_attribute_wrong_decl_type_str) << Attr << "fields";
        return false;
      }
      return true;
    }

    AttrHandling handleDeclAttribute(Sema& S, Decl* D, const ParsedAttr& Attr) const override {
      // Attach an annotate attribute to the Decl.
      D->addAttr(AnnotateAttr::Create(S.Context, "ignore", nullptr, 0, Attr.getRange()));
      return AttributeApplied;
    }
  };
} // namespace

static ParsedAttrInfoRegistry::Add<ExampleAttrInfo> Y("ignore", "");