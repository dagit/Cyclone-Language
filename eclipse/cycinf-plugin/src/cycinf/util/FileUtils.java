package cycinf.util;

import java.io.*;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;

public abstract class FileUtils {
	private FileUtils() {}

	public static String readFile(IFile theFile) throws CoreException,
		IOException {
		InputStream is = theFile.getContents();
		Reader r = new InputStreamReader(is);
		StringBuilder data = new StringBuilder();
		/*
		 * buffer size hack -- if there's a good implementation of
		 * available, then we'll go through the buffer in one try
		 */
		char[] buffer = new char[Math.max(1024, is.available())];
		int charsRead;
		while ((charsRead = r.read(buffer)) >= 0)
			data.append(buffer, 0, charsRead);
		return data.toString();
	}
}
