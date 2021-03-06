/* Stream type for the ParserStream */
type printerStream = LazyStream.t(string);

let printer = (s: Parser.parserStream) => {
  let indentation = ref(0);

  let stringifyKind = (kind: Parser.ruleKind) : string => {
    switch (kind) {
    | StyleRule => "STYLE_RULE"
    | CharsetRule => "CHARSET_RULE"
    | ImportRule => "IMPORT_RULE"
    | MediaRule => "MEDIA_RULE"
    | FontFaceRule => "FONT_FACE_RULE"
    | PageRule => "PAGE_RULE"
    | KeyframesRule => "KEYFRAMES_RULE"
    | KeyframeRule => "KEYFRAME_RULE"
    | MarginRule => "MARGIN_RULE"
    | NamespaceRule => "NAMESPACE_RULE"
    | CounterStyleRule => "COUNTER_STYLE_RULE"
    | SupportsRule => "SUPPORTS_RULE"
    | DocumentRule => "DOCUMENT_RULE"
    | FontFeatureValuesRule => "FONT_FEATURE_VALUES_RULE"
    | ViewportRule => "VIEWPORT_RULE"
    | RegionStyleRule => "REGION_STYLE_RULE"
    }
  };

  let stringifyNode = (node: Parser.node) : string => {
    let indentImmediate = ref(indentation^);
    let nodeOutput = switch (node) {
    | RuleStart(kind) => {
      indentation := indentation^ + 1;
      "[RULE_START, " ++ stringifyKind(kind) ++ "]"
    }
    | RuleEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[RULE_END]"
    }
    | RuleName(str) => "[RULE_NAME, '" ++ str ++ "']"
    | Selector(str) => "[SELECTOR, '" ++ str ++ "']"
    | ParentSelector => "[PARENT_SELECTOR]"
    | UniversalSelector => "[UNIVERSAL_SELECTOR]"
    | CompoundSelectorStart => {
      indentation := indentation^ + 1;
      "[COMPOUND_SELECTOR_START]"
    }
    | CompoundSelectorEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[COMPOUND_SELECTOR_END]"
    }
    | SpaceCombinator => "[SPACE_COMBINATOR]"
    | DoubledChildCombinator => "[DOUBLED_CHILD_COMBINATOR]"
    | ChildCombinator => "[CHILD_COMBINATOR]"
    | NextSiblingCombinator => "[NEXT_SIBLING_COMBINATOR]"
    | SubsequentSiblingCombinator => "[SUBSEQUENT_SIBLING_COMBINATOR]"
    | Property(str) => "[PROPERTY, '" ++ str ++ "']"
    | Value(str) => "[VALUE, '" ++ str ++ "']"
    | CompoundValueStart => {
      indentation := indentation^ + 1;
      "[COMPOUND_VALUE_START]"
    }
    | CompoundValueEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[COMPOUND_VALUE_END]"
    }
    | Condition(str) => "[CONDITION, '" ++ str ++ "']"
    | FunctionStart(str) => {
      indentation := indentation^ + 1;
      "[FUNCTION_START, '" ++ str ++ "']"
    }
    | FunctionEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[FUNCTION_END]"
    }
    | AnimationName(str) => "[ANIMATION_NAME, '" ++ str ++ "']"
    | SelectorRef(_) => "[SELECTOR_REF, ref]"
    | PropertyRef(_) => "[PROPERTY_REF, ref]"
    | ValueRef(_) => "[VALUE_REF, ref]"
    | PartialRef(_) => "[PARTIAL_REF, ref]"
    | StringStart(str) => {
      indentation := indentation^ + 1;
      "[STRING_START, '" ++ str ++ "']"
    }
    | StringEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[STRING_END]"
    }
    | AttributeSelectorStart(kind) =>
      "[ATTRIBUTE_SELECTOR_START, " ++ string_of_int(Parser.attributeSelectorKindToJs(kind)) ++ "]"
    | AttributeSelectorEnd => "[ATTRIBUTE_SELECTOR_END]"
    | AttributeName(str) => "[ATTRIBUTE_NAME, '" ++ str ++ "']"
    | AttributeNameRef(_) => "[ATTRIBUTE_NAME_REF, ref]"
    | AttributeOperator(str) => "[ATTRIBUTE_OPERATOR, '" ++ str ++ "']"
    | AttributeValue(str) => "[ATTRIBUTE_VALUE, '" ++ str ++ "']"
    | AttributeValueRef(_) => "[ATTRIBUTE_VALUE_REF, ref]"
    | ConditionRef(_) => "[CONDITION_REF, ref]"
    | CompoundConditionStart => {
      indentation := indentation^ + 1;
      "[COMPOUND_CONDITION_START]"
    }
    | CompoundConditionEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[COMPOUND_CONDITION_END]"
    }
    | ConditionGroupStart => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[CONDITION_GROUP_START]"
    }
    | ConditionGroupEnd => {
      indentation := indentation^ - 1;
      indentImmediate := indentImmediate^ - 1;
      "[CONDITION_GROUP_END]"
    }
    | EOF => "/eof"
    };

    let indent = String.make(indentImmediate^ * 2, ' ');
    indent ++ nodeOutput
  };

  let next: [@bs] (unit => option(string)) = [@bs] (() => {
    switch (LazyStream.next(s)) {
      | Some(node) => Some(stringifyNode(node))
      | None => None
    }
  });

  LazyStream.from(next)
};

/* collects all emissions of a stream in an array */
let printToStr = (s: printerStream) => {
  let rec populate = (acc: string) : string => {
    switch (LazyStream.next(s)) {
    | Some(str) => populate(acc ++ "\n" ++ str)
    | None => acc
    }
  };

  populate("")
};
