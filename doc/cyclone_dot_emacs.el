;; WARNING: We are hacking C++ mode rather than creating our own.
;; This is a very bad idea, except that we never write C++ code. :-)
(defun my-c++-hook ()
  (modify-syntax-entry ?\` "w" c++-mode-syntax-table))
(setq auto-mode-alist
      (cons '("\\.h$"       . c++-mode) 
      (cons '("\\.pop[l]?$" . c++-mode) 
      (cons '("\\.cyl"      . c++-mode) 
      (cons '("\\.cyc"      . c++-mode)
      (cons '("\\.cys"      . c++-mode)
      (cons '("\\.y"        . c++-mode)
      auto-mode-alist)))))))
(font-lock-add-keywords 'c++-mode ; reserved words not in C++ and not types
    (list 
      "\\\<rnew>\\\>"
      "\\\<new>\\\>"
      "\\\<malloc>\\\>"
      "\\\<rmalloc>\\\>"
      "\\\<region>\\\>"      
      "\\\<xtunion\\\>"  ; deprecated
      "\\\<tunion\\\>"   ; deprecated
      "\\\<datatype\\\>"
      "\\\<extensible\\\>"
      "\\\<zeroterm\\\>"
      "\\\<nozeroterm\\\>"
      "\\\<resetable\\\>"
      "\\\<tagged\\\>"
      "\\\<fallthru\\\>"
      '("\\\<NULL\\\>" (0 'font-lock-constant-face t t))
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
     ; not really a type, but syntactically it is (and a wildcard pattern, 'tho)
     "_"
     ; C convention -- use _t to end your typedefs
     "\\sw+_t" 
     ; library types -- feel free to add more
     "FILE" 
     ))
(setq c++-mode-hook 'my-c++-hook)
