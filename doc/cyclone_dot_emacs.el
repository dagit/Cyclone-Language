;; POPCORN AND CYCLONE
(defun my-c++-hook ()
  (modify-syntax-entry ?\` "w" c++-mode-syntax-table))
(setq auto-mode-alist
      (cons '("\\.h$"       . c++-mode) 
      (cons '("\\.pop[l]?$" . c++-mode) 
      (cons '("\\.cyl"      . c++-mode) 
      (cons '("\\.cyc"      . c++-mode)
      (cons '("\\.y"        . c++-mode)
      auto-mode-alist))))))
(font-lock-add-keywords 'c++-mode ; reserved words not in C++ and not types
    (list 
      "\\\<xenum\\\>" 
      "\\\<fallthru\\\>"
      "\\\<default\\\>"
      "\\\<abstract\\\>"
      "\\\<codegen\\\>"
      "\\\<splice\\\>"
      "\\\<cut\\\>"
      "\\\<fill\\\>"
      "\\\<where\\\>"
      '("\\\<null\\\>" (0 'font-lock-constant-face t t))
      ; color the pattern like a constant, except identifiers starting
      ; with lower-case are variables.
      '("\\(\\\<let\\\>\\)\\([^=\n]*\\)=" 
	 (1 'font-lock-type-face) 
	 (2 'font-lock-constant-face)
	 ("[^A-Za-z0-9_]\\([a-z][A-Za-z0-9_]*\\)"  
	  (cond (t (goto-char (match-end 1)) (match-end 2)))
	  nil
	  (1 font-lock-variable-name-face t t)))
      '("\\\<let\\\>" . 'font-lock-type-face))
    t)

(setq c++-font-lock-extra-types 
   '(; tyvars start with `
     "`\\sw+" 
     ; not really a type, but this gets the non-pattern (add _ for Popcorn)
     "_"
     ; C convention -- use _t to end your typedefs
     "\\sw+_t" 
     ; library types -- feel free to add more
     "FILE" "string" "list" "Set" "Dict"
     ))
(setq c++-mode-hook 'my-c++-hook)
