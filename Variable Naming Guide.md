# Variable Naming Guide for Math Expression Solver

This document summarizes the main variable naming conventions used across the project, including their intended meaning, type, and usage context. Following these conventions ensures readability, maintainability, and consistency.

- **🟩 Green** — Correct / Fixed position
- **🟨 Yellow** — Partially correct / Present but misplaced
- **🟥 Red** — Not present / Forbidden

---

## 1. Constraints

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `constraintsMap` | `std::unordered_map<char, Constraint>` | Collection of all constraints for symbols in expressions. Keyed by character. | `constraintsMap['+'].minCount() = 1;` |
| `constraint` | `Constraint` | Single constraint representing rules for one character. | `Constraint plusConstraint('+');` |
| `lhsConstraintsMap` | `std::unordered_map<char, Constraint>` | Subset of `constraintsMap` specific to LHS during generation. | `lhsConstraintsMap = constraintsMap;` |

---

## 2. Expressions & Color Lines

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `expressions` | `std::vector<std::string>` | Multiple expression strings, e.g., historical input or candidate list. | `expressions = {"12+34=46", "7*5=35"};` |
| `expressionColors` | `std::vector<std::string>` | Color annotations for each expression line. | `expressionColors = {"grgrrgrr", "ygrgrr"};` |
| `exprLine` | `std::string` | Single expression line being processed. | `exprLine = "12+34=46";` |
| `exprColorLine` | `std::string` | Color annotation for a single expression line. | `exprColorLine = "grgrrgrr";` |

**Example Usage:**

```cpp
std::string exprLine = expressions[0];           // "12+34=46"
std::string exprColorLine = expressionColors[0]; // "grgrrgrr"
```

---

## 3. Characters

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `exprChar` | `char` | Single character in an expression. | `'1', '+', '2'` |
| `exprColorChar` | `char` | Single color annotation corresponding to `exprChar`. | `'g', 'y', 'r'` |
| `requiredCharsAtPos` | `std::vector<char>` | Characters that must appear at specific positions (green positions) during LHS generation. | `requiredCharsAtPos[0] = '1';` |

---

## 4. Operators

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `operatorsSet` | `std::unordered_set<char>` | Set of allowed operators (`+`, `-`, `*`, `/`, `^`) for expression evaluation or generation. | `{'+','-','*'}` |
| `operator` | `char` | Single operator character being processed. | `'+'`|

**Example Usage:**

```cpp
if (operatorsSet.count('+')) {
    std::cout << "Addition allowed\n";
}
```

---

## 5. Tokens

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `tokensList` | `std::vector<Expression::Token>` | A sequence of tokens representing a complete expression or partial LHS. | `tokensList = { {Digit, "12"}, {Operator, "+"}, {Digit, "34"} };` |
| `token` | `Expression::Token` | Single token, either a digit block or an operator. | `Expression::Token t{Digit, "12"};` |
| `currentTokens` | `std::vector<Expression::Token>` | Tokens currently being generated in DFS recursion. | See `_dfsGenerateLeftTokens`. |
| `lhsCandidatesList` | `std::vector<std::vector<Expression::Token>>` | List of all possible LHS token sequences generated. | Multiple vector sequences of tokens. |
| `lastToken` | `Expression::Token*` | Pointer to the most recently added token in the generation process. Used for syntax validation (e.g., avoid consecutive operators). | `lastToken = &currentTokens.back();` |
| `previousToken` | `Expression::Token*` | Pointer to the previous token (one before lastToken). Helps detect invalid patterns such as "++" or "*/". | `previousToken = &currentTokens[currentTokens.size() - 2];` |
| `previous_2Token` | `Expression::Token*` | Pointer to the token before previousToken. Used when checking context-sensitive patterns, e.g., validating multi-digit sequences or nested signs. | `previous_2Token = &currentTokens[currentTokens.size() - 3];` |

**Example Usage:**

```cpp
std::vector<Expression::Token> currentTokens;
currentTokens.push_back({TokenType::Digit, "12"});
currentTokens.push_back({TokenType::Operator, "+"});
currentTokens.push_back({TokenType::Digit, "34"});

Expression::Token* lastToken = &currentTokens.back();                        // "34"
Expression::Token* previousToken = &currentTokens[currentTokens.size()-2];   // "+"
Expression::Token* previous_2Token = &currentTokens[currentTokens.size()-3]; // "12"
```

---

## 6. Positions, Lengths, and Indices

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `lhsLength` | `int` | Length of the left-hand side of an expression. | `lhsLength = 5;` |
| `rhsLength` | `int` | Length of the right-hand side of an expression. | `rhsLength = 2;` |
| `usedLength` | `int` | Current number of characters used in DFS generation. | `usedLength = 3;` |
| `dfsDepth` | `int` | Depth of recursion in `_dfsGenerateLeftTokens`. | `dfsDepth = 2;` |
| `eqSignPositionsList` | `std::vector<int>` | Candidate positions for the '=' sign, respecting green positions. | `{3, 4, 5}` |
| `currentPosition` | `int` | Current character index in LHS being processed. | `currentPosition = 0;` |
| `sumDigits` | `int` | Temporary sum of digits in a composition during RHS feasibility check. | `sumDigits = 5;` |

---

## 7. Evaluation and Results

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `lhsString` | `std::string` | String representation of the left-hand side expression (e.g., "12+34"). | `lhsString = "12+34";` |
| `rhsString` | `std::string` | String representation of the RHS value, formatted according to integer/floating-point rules. | `rhsString = "46";` |
| `lhsResult` | `double` | Result of evaluating the left-hand side expression. | `lhsResult = 46.0;` |
| `rhsResult` | `double` | Numeric value of the right-hand side expression (if applicable). Used to verify equality. | `rhsResult = 46.0;` |
| `islhsResultInt` | `bool` | Whether `lhsResult` is an integer. | `islhsResultInt = true;` |
| `finalCandidatesList` | `std::vector<std::string>` | Valid candidate expressions after evaluation and constraint checks. | `{"12+34=46"}` |

**Example Usage:**

```cpp
std::string lhsString = "12+34";
std::string rhsString;
double lhsResult = 0.0;
double rhsResult = 0.0;
bool islhsResultInt = false;

// Evaluate LHS
lhsResult = ExpressionEvaluator::evaluate(lhsString);
islhsResultInt = std::floor(lhsResult) == lhsResult;

// Format RHS based on result type
rhsString = islhsResultInt ? std::to_string((int)lhsResult) : std::to_string(lhsResult);
rhsResult = std::stod(rhsString);

std::vector<std::string> finalCandidatesList = { lhsString + "=" + rhsString };
```

---

## 8. Miscellaneous

| Variable | Type | Description | Example |
|----------|------|-------------|---------|
| `forbiddenSet` | `std::unordered_set<char>` | Characters that are forbidden in the final expression (minCount = maxCount = 0). | `forbiddenSet.insert('*');` |

---

## 9. Casting

| Expression | Purpose | Example |
|------------|---------|---------|
| `static_cast<int>(...)` | Convert a value to `int` safely, often used to convert `char` or `size_t` to `int`. | `int idx = static_cast<int>(token.value.size());` |

---

## Notes

1. **Consistency**: Always use plural form for collections (`expressions`, `tokensList`) and singular for individual items (`exprLine`, `token`).  
2. **Clarity**: Use descriptive names that indicate the type and purpose (`lhsConstraintsMap`, `requiredCharsAtPos`).  
3. **DFS/Recursion Context**: Variables like `dfsDepth`, `currentTokens`, `usedLength` are internal to recursion and should be named to indicate their role in the generation process.  
4. **Constraints**: `Constraint` objects always represent rules for a single symbol, while `constraintsMap` aggregates all of them for a given round or generation.  

---

