;; WARNING: We are hacking C++ mode rather than creating our own.
;; This is a very bad idea, except that we never write C++ code. :-)
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
      "\\\<xtunion\\\>" 
      "\\\<tunion\\\>" 
      "\\\<fallthru\\\>"
      "\\\<default\\\>"
      "\\\<abstract\\\>"
      "\\\<codegen\\\>"
      "\\\<splice\\\>"
      "\\\<cut\\\>"
      "\\\<fill\\\>"
      '("\\\<null\\\>" (0 'font-lock-constant-face t t)))
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
