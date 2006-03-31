package cycinf.ui;

// I know it's bad to extend something "internal", but this achieves what we want!
// It also works fine extending the Text Editor.
public class CycloneEditor extends org.eclipse.ui.editors.text.TextEditor /*org.eclipse.cdt.internal.ui.editor.CEditor*/ {}
