;;; marlais.el --- Dylan process in a buffer.  Adapted from cmuscheme.el

;; Author: Joseph N. Wilson <jnw@cis.ufl.edu>
;; Keywords: processes, dylan

;; This file is lifted almost verbatim from cmuscheme.el, which is
;; a part of GNU Emacs written by Olin Shivers <olin.shivers@cs.cmu.edu>

;; cmuscheme.el is (and thus marlais.el is)
;; Copyright (C) 1988 Free Software Foundation, Inc.
;; marlais.el is Copyright (C) 1994 J.N. Wilson under the same terms and
;; conditions as cmuscheme.el.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Commentary:

;;;    This is a customisation of comint-mode (see comint.el)

;; YOUR .EMACS FILE
;;=============================================================================
;; Some suggestions for your .emacs file.
;;
;; ; If marlais.el lives in some non-standard directory, you must tell emacs
;; ; where to get it. This may or may not be necessary.
;; (setq load-path (cons (expand-file-name "~jones/lib/emacs") load-path))
;;
;; ; Autoload run-dylan from file marlais.el
;; (autoload 'run-dylan "marlais"
;;           "Run an inferior Dylan process."
;;           t)
;;
;; ; Files ending in ".dyl" are Dylan source, 
;; ; so put their buffers in dylan-mode.
;; (setq auto-mode-alist 
;;       (cons '("\\.dyl$" . dylan-mode)  
;;             auto-mode-alist))
;;
;; ; Define C-c t to run my favorite command in inferior dylan mode:
;; (setq marlais-load-hook
;;       '((lambda () (define-key inferior-dylan-mode-map "\C-ct"
;;                                'favorite-cmd))))

;;;
;;;  Revision History:
;;
;; 1995/06/19 jnw@cis.ufl.edu
;; Modified call to full-copy-sparse-keymap for grabbing comint mode
;; bindings in inferior-dylan-mode-map to be call to copy-keymap
;; It appears full-copy-sparse-keymap is deprecated.
;;

;;;
;;; Code:

(require 'dylan-mode)
(require 'comint)

;;; INFERIOR DYLAN MODE STUFF
;;;============================================================================

(defvar inferior-dylan-mode-hook nil
  "*Hook for customising inferior-dylan mode.")
(defvar inferior-dylan-mode-map nil)

(cond ((not inferior-dylan-mode-map)
       (setq inferior-dylan-mode-map
	     (copy-keymap comint-mode-map))
       (define-key inferior-dylan-mode-map "\C-x\C-e" 'comint-send-input)
       (define-key inferior-dylan-mode-map "\C-c\C-l" 'dylan-load-file)
       (define-key inferior-dylan-mode-map "\M-\C-m" 'dylan-send-from-prompt)
       (define-key inferior-dylan-mode-map "\C-m" 'dylan-send-input)
       (dylan-mode-commands inferior-dylan-mode-map))) 


;; Install the process communication commands in the dylan-mode keymap.
(define-key dylan-mode-map "\C-c\C-r" 'dylan-send-region)
(define-key dylan-mode-map "\C-c\M-r" 'dylan-send-region-and-go)
(define-key dylan-mode-map "\C-c\C-z" 'switch-to-dylan)
(define-key dylan-mode-map "\C-c\C-l" 'dylan-load-file)


(defvar dylan-buffer)

(defun inferior-dylan-mode ()
  "Major mode for interacting with an inferior Dylan process.
The following commands are available:
\\{inferior-dylan-mode-map}

A Dylan process can be fired up with M-x run-dylan.

Customisation: Entry to this mode runs the hooks on comint-mode-hook and
inferior-dylan-mode-hook (in that order).

You can send text to the inferior Dylan process from other buffers containing
Dylan source.  
    switch-to-dylan switches the current buffer to the Dylan process buffer.
    dylan-send-region sends the current region to the Dylan process.
    dylan-send-region-and-go switches to the Dylan process buffer after
      sending text.
For information on running multiple processes in multiple buffers, see
documentation for variable dylan-buffer.

Commands:
Return after the end of the process' output sends the text from the 
    end of process to point.
Return before the end of the process' output copies the sexp ending at point
    to the end of the process' output, and sends it.
Delete converts tabs to spaces as it moves back.
Tab indents for Dylan; with argument, shifts rest
    of expression rigidly with the current line.
C-M-q does Tab on each line starting within following expression.
Paragraphs are separated only by blank lines.  Semicolons start comments.
If you accidentally suspend your process, use \\[comint-continue-subjob]
to continue it."
  (interactive)
  (comint-mode)
  ;; Customise in inferior-dylan-mode-hook
  (setq comint-prompt-regexp "^\? *") ; OK for Marlais
  (dylan-mode-variables)
  (setq major-mode 'inferior-dylan-mode)
  (setq mode-name "Inferior Dylan")
  (setq mode-line-process '(": %s"))
  (use-local-map inferior-dylan-mode-map)
  (setq comint-input-filter (function dylan-input-filter))
  (setq comint-input-sentinel (function ignore))
  (setq comint-get-old-input (function dylan-get-old-input))
  (run-hooks 'inferior-dylan-mode-hook))


(defvar inferior-dylan-filter-regexp "\\`\\s *\\S ?\\S ?\\s *\\'"
  "*Input matching this regexp are not saved on the history list.
Defaults to a regexp ignoring all inputs of 0, 1, or 2 letters.")

(defun dylan-input-filter (str)
  "Don't save anything matching inferior-dylan-filter-regexp"
  (not (string-match inferior-dylan-filter-regexp str)))

(defun dylan-get-old-input ()
  "Snarf the sexp ending at point"
  (save-excursion
    (let ((end (point)))
      (backward-sexp)
      (buffer-substring (point) end))))

(defun dylan-args-to-list (string)
  (let ((where (string-match "[ \t]" string)))
    (cond ((null where) (list string))
	  ((not (= where 0))
	   (cons (substring string 0 where)
		 (dylan-args-to-list (substring string (+ 1 where)
						 (length string)))))
	  (t (let ((pos (string-match "[^ \t]" string)))
	       (if (null pos)
		   nil
		 (dylan-args-to-list (substring string pos
						 (length string)))))))))

(defvar dylan-program-name "marlais"
  "*Program invoked by the run-dylan command")

(defun run-dylan (cmd)
  "Run an inferior Dylan process, input and output via buffer *dylan*.
If there is a process already running in *dylan*, just switch to that buffer.
With argument, allows you to edit the command line (default is value
of dylan-program-name).  Runs the hooks from inferior-dylan-mode-hook
\(after the comint-mode-hook is run).
\(Type \\[describe-mode] in the process buffer for a list of commands.)"

  (interactive (list (if current-prefix-arg
			 (read-string "Run Dylan: " dylan-program-name)
			 dylan-program-name)))
  (if (not (comint-check-proc "*dylan*"))
      (let ((cmdlist (dylan-args-to-list cmd)))
	(set-buffer (apply 'make-comint "dylan" (car cmdlist)
			   nil (cdr cmdlist)))
	(inferior-dylan-mode)))
  (setq dylan-buffer "*dylan*")
  (switch-to-buffer "*dylan*"))

(defun dylan-get-old-input ()
  "Snarf the text between prompt and point"
  (save-excursion
    (let ((end (point)))
      (comint-prev-prompt)
      (buffer-substring (point) end))))

(defun dylan-send-string (string)
  (let ((index (string-match "[^\n]\n" string)))
    (while index
      (progn
	(setq index (+ index 1))
	(comint-send-string (dylan-proc) (substring string 0 index))
	(setq string (substring string index))
	(setq index (string-match "[^\n]\n" string))))
    (comint-send-string (dylan-proc) string)))

(defun dylan-send-region (start end)
  "Send the current region to the inferior Dylan process."
  (interactive "r")
  ;;  (comint-send-string (dylan-proc) (buffer-substring start end))
  ;;  (comint-send-string (dylan-proc) "\n"))
  (dylan-send-string (buffer-substring start end)))


(defun dylan-send-input () 
  "Send input to the dylan buffer."
  (interactive)
  ;; Note that the input string does not include its terminal newline.
  (let ((proc (get-buffer-process (current-buffer))))
    (if (not proc) (error "Current buffer has no process")
	(let* ((pmark (process-mark proc))
	       (pmark-val (marker-position pmark))
	       (input (if (>= (point) pmark-val)
			  (progn (if comint-eol-on-send (end-of-line))
				 (buffer-substring pmark (point)))
			(let ((copy (funcall comint-get-old-input)))
			  (goto-char pmark)
			  (insert copy)
			  copy))))
          (if comint-process-echoes
              (delete-region pmark (point))
            (insert ?\n))
	  (if (funcall comint-input-filter input)
	      (ring-insert comint-input-ring input))
	  (funcall comint-input-sentinel input)
	  (dylan-send-string input)
	  (comint-send-string (dylan-proc) "\n")
	  (setq comint-input-ring-index nil)
	  (set-marker comint-last-input-start pmark)
	  (set-marker comint-last-input-end (point))
	  (set-marker (process-mark proc) (point))))))

(defun dylan-send-from-prompt ()
  "Send input starting at the last prompt to the inferior Dylan process."
  (interactive)
  (dylan-send-region (save-excursion (comint-prev-prompt 1) (point)) (point)))

(defun switch-to-dylan (eob-p)
  "Switch to the dylan process buffer.
With argument, positions cursor at end of buffer."
  (interactive "P")
  (if (get-buffer dylan-buffer)
      (pop-to-buffer dylan-buffer)
      (error "No current process buffer. See variable dylan-buffer."))
  (cond (eob-p
	 (push-mark)
	 (goto-char (point-max)))))

(defun dylan-send-region-and-go (start end)
  "Send the current region to the inferior Dylan process.
Then switch to the process buffer."
  (interactive "r")
  (dylan-send-region start end)
  (switch-to-dylan t))

(defvar dylan-source-modes '(dylan-mode)
  "*Used to determine if a buffer contains Dylan source code.
If it's loaded into a buffer that is in one of these major modes, it's
considered a dylan source file by dylan-load-file.
Used by these commands to determine defaults.")

(defvar dylan-prev-l/c-dir/file nil
  "Caches the last (directory . file) pair.
Caches the last pair used in the last dylan-load-file command.
Used for determining the default in the next one.")

(defun dylan-load-file (file-name)
  "Load a Dylan file into the inferior Dylan process."
  (interactive (comint-get-source "Load Dylan file: " dylan-prev-l/c-dir/file
				  dylan-source-modes t)) ; T because LOAD 
                                                          ; needs an exact name
  (comint-check-source file-name) ; Check to see if buffer needs saved.
  (setq dylan-prev-l/c-dir/file (cons (file-name-directory    file-name)
				       (file-name-nondirectory file-name)))
  (comint-send-string (dylan-proc) (concat "load (\""
					    file-name
					    "\"\);\n")))


(defvar dylan-buffer nil "*The current dylan process buffer.

MULTIPLE PROCESS SUPPORT
===========================================================================
Dylan.el supports, in a fairly simple fashion, running multiple Dylan
processes. To run multiple Dylan processes, you start the first up with
\\[run-dylan]. It will be in a buffer named *dylan*. Rename this buffer
with \\[rename-buffer]. You may now start up a new process with another
\\[run-dylan]. It will be in a new buffer, named *dylan*. You can
switch between the different process buffers with \\[switch-to-buffer].

Commands that send text from source buffers to Dylan processes --
like dylan-send-region  -- have to choose a
process to send to, when you have more than one Dylan process around. This
is determined by the global variable dylan-buffer. Suppose you
have three inferior Dylans running:
    Buffer	Process
    foo		dylan
    bar		dylan<2>
    *dylan*    dylan<3>
If you do a \\[dylan-send-region-and-go] command on some Dylan source
code, what process do you send it to?

- If you're in a process buffer (foo, bar, or *dylan*), 
  you send it to that process.
- If you're in some other buffer (e.g., a source file), you
  send it to the process attached to buffer dylan-buffer.
This process selection is performed by function dylan-proc.

Whenever \\[run-dylan] fires up a new process, it resets dylan-buffer
to be the new process's buffer. If you only run one process, this will
do the right thing. If you run multiple processes, you can change
dylan-buffer to another process buffer with \\[set-variable].

More sophisticated approaches are, of course, possible. If you find yourself
needing to switch back and forth between multiple processes frequently,
you may wish to consider ilisp.el, a larger, more sophisticated package
for running inferior Lisp and Dylan processes. The approach taken here is
for a minimal, simple implementation. Feel free to extend it.")

(defun dylan-proc ()
  "Returns the current dylan process. See variable dylan-buffer."
  (let ((proc (get-buffer-process (if (eq major-mode 'inferior-dylan-mode)
				      (current-buffer)
				      dylan-buffer))))
    (or proc
	(error "No current process. See variable dylan-buffer"))))


;;; Do the user's customisation...

(defvar marlais-load-hook nil
  "This hook is run when marlais is loaded in.
This is a good place to put keybindings.")
	
(run-hooks 'marlais-load-hook)


;;; CHANGE LOG
;;; 11/02/1994 jnw@cis.ufl.edu
;;; - Fixed dylan-send-region to work correctly with Marlais lexical analyzer.
;;;   Added dylan-send-region-recursively to split input correctly.
;;;


(provide 'marlais)

;;; marlais.el ends here


