[![C++](https://img.shields.io/badge/Built%20with-C++-%2300599C.svg?style=flat-square&logo=c%2B%2B&logoColor=white)](#)

---

## 💡 About This Project

This project is only for private use.  

---

## ⚙️ Project Features 專案特點

本專案以 C++20 編寫，用於解析與求解《MATH EXPRESSions》遊戲的所有可能正確算式組合。
為了模擬遊戲邏輯與提供高效驗證，本程式具備以下主要特點：

1. 🧠 使用 DFS（深度優先搜尋）與剪枝演算法

   - 遞迴生成左側（LHS）可能的運算式。

   - 使用 Token 的方式生成 LHS，而非逐字暴力破解。

   - 依據運算子、數字與遊戲的規則即時剪枝，避免無效組合。

2. ⚡ 支援快速驗證與結果過濾

   - 利用 ExpressionValidator 判斷生成的算式是否合理合法。

   - 將已猜測過的顏色回饋統整，記錄至 Constraint 模組，與生成出的所有候選比對，進一步高效篩選出可能的合理候選。

3. 🔁 具備 Undo / Backtrack 機制

   - 如果輸入錯誤，可回溯至先前狀態，不需從頭開始輸入、計算來建立整個候選清單，節省記憶體與運算時間。

4. 🧩 模組化結構與命名規範

   - 清楚區分模組：Generator、Validator、Evaluator、ConstraintUtils。

   - 變數名稱命名清楚，具有語意及類別：constraintsMap、operatorsSet、tokensList。

   - 採用一致變數命名規則（如 constraintsMap, expressionColors, lhsResult 等）。

5. 🎨 顏色邏輯完全對應遊戲規則

   - 綠色（Green）：字元正確且位置正確。

   - 黃色（Yellow）：字元正確但位置錯誤。

   - 紅色（Red）：字元不存在或超出允許次數。

6. 🧾 數學運算支援與格式化

   - 自動判斷計算結果是否符合規則（是否能整除、是否為正整數）。

   - 具有精度誤差處理（double 整數與整數運算可能產生小數）。

7. 🧮 結果生成與驗證流程完全自動化

   - 自動組合出完整表達式（"12+34=46"）。

   - 驗證每個候選是否符合顏色限制與運算結果。

8. 📚 支援彩色 Console 顯示及檔案 Log

   - 支援跨平台（Windows 及 Unix）彩色 Console 顯示。

   - 具有檔案 Logging 的功能。

   - 可自訂 Log 是否換行、是否具有時間戳記、是否覆寫預設顏色。

---

## 🔄 Implementation Workflow 實作流程

以下為整體執行架構與邏輯流程：

1. 初始化階段

   - 玩家輸入允許的長度（例如 8）及運算子集合（例如 +, -, *, /）。

   - 初始化 initializeConstraintsMap() 用來記錄所有字元對應之約束。

2. 左側候選生成（DFS）

   - 由 CandidateGenerator::_dfsGenerateLeftTokens() 遞迴生成 LHS token 序列。

   - 使用 lastToken、previousToken 等指標確保所生成的候選算式符合語法及規則。

   - 若 token 組合無效則立即剪枝返回。

3. 算式拼接與求值

   - 將 LHS token 串接成字串（lhsString）。

   - 使用 ExpressionEvaluator::evaluate() 計算結果 (lhsResult)。

   - 根據結果生成 rhsString 並構造完整表達式 "LHS=RHS"。

4. 驗證與過濾階段

   - 呼叫 ExpressionValidator::isCandidateValid() 檢查是否符合顏色與約束規則。

   - 將通過檢查的候選加入至 finalCandidatesList。

5. 結果輸出

   - 將所有符合條件的算式輸出。

   - 等待玩家下一步的輸入。

---

## 📊 執行流程示意圖（簡化）

```c
[初始化階段]
      ↓
[DFS 遞迴生成 LHS Tokens]
      ↓
[格式化 LHS → 計算 lhsResult]
      ↓
[生成 RHS → 格式化 rhsString]
      ↓
[驗證顏色限制與算式合法性]
      ↓
[加入 finalCandidatesList]
```

---

## 🛡️ About MATH EXPRESSions

"[MATH EXPRESSions](https://store.steampowered.com/app/1953970/)" is a Steam game, similar to a complex version of 1A2B.  

Rules:

1. The equation always holds true.
2. Operator '=' can only appear once.
3. 0-9 and operators can appear 0, 1, or more times.
4. Green indicates the character is in the correct position.
5. Yellow indicates the character appears but in the wrong position.
6. Red indicates the character should not appear or appears too many times.

---

## 📜 Third-Party Licenses

This project uses [**fmt**](https://github.com/fmtlib/fmt), licensed under the [MIT](https://github.com/fmtlib/fmt/blob/master/LICENSE).

- 僅供個人學術研究使用，可相互探討，禁止抄襲（包含部分抄襲）
- 禁止搬運、複製或儲存於其他儲存庫中
- 禁止當作作業 Project
- 禁止當作作業 Project
- 禁止當作作業 Project

> This repository **does not provide any license or redistribution rights** for its own code.  
> It is intended for **private use only**.

---

## 👤 About Author

- [Discord](https://discord.gg/GDMSyVt)
- [Twitch](https://bit.ly/DragonTakiTwitch)
- [YouTube](https://bit.ly/DragonTakiYTNew)
- [Twitter](https://twitter.com/MacroDragonTaki)
- [Fur Affinity](https://bit.ly/DragonTakiFA)
- [巴哈姆特](https://bit.ly/DragonTakiBaha)
