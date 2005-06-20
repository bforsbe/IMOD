package etomo.util;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Hashtable;

import etomo.ApplicationManager;
import etomo.EtomoDirector;
import etomo.process.SystemProgram;
import etomo.type.AxisID;
import etomo.type.ConstEtomoNumber;
import etomo.type.EtomoNumber;

/**
* <p>Description: Runs montagesize on .st files.  Creates once instance per file
* Rereads only when file has changed.</p>
* 
* <p>Copyright: Copyright (c) 2005</p>
*
*<p>Organization:
* Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEM),
* University of Colorado</p>
* 
* @author $Author$
* 
* @version $Revision$
*/
public class Montagesize {
  public static  final String  rcsid =  "$Id$";
  private static final String fileExtension = ".st";
  //
  //n'ton member variables
  //
  private static Hashtable instances = new Hashtable();
  //
  //member variables to prevent unnecessary reads
  //
  private FileModifiedFlag modifiedFlag;
  //
  //other member variables
  //
  private File file = null;
  private boolean fileExists = false;
  private EtomoNumber x = new EtomoNumber(EtomoNumber.INTEGER_TYPE);
  private EtomoNumber y = new EtomoNumber(EtomoNumber.INTEGER_TYPE);
  private EtomoNumber z = new EtomoNumber(EtomoNumber.INTEGER_TYPE);
  AxisID axisID;

  //
  //n'ton functions
  //
  /**
   * private constructor
   */
  private Montagesize(File file, AxisID axisID) {
    this.file = file;
    this.axisID = axisID;
    modifiedFlag = new FileModifiedFlag(file);
  }
  /**
   * Function to get an instance of the class
   * @param directory
   * @param datasetName
   * @param axisID
   * @return
   */
  public static Montagesize getInstance(String directory, String datasetName,
      AxisID axisID) {
    File keyFile = makeFile(directory, datasetName, axisID);
    String key = makeKey(keyFile);
    Montagesize montagesize = (Montagesize) instances.get(key);
    if (montagesize == null) {
      return createInstance(key, keyFile, axisID);
    }
    return montagesize;
  }
  /**
   * Function to create and save an instance of the class.  Just returns the
   * instance if it already exists.
   * @param key
   * @param file
   * @param axisID
   * @return
   */
  private static synchronized Montagesize createInstance(String key,
      File file, AxisID axisID) {
    Montagesize montagesize = (Montagesize) instances.get(key);
    if (montagesize != null) {
      return montagesize;
    }
    montagesize = new Montagesize(file, axisID);
    instances.put(key, montagesize);
    montagesize.selfTestInvariants();
    return montagesize;
  }
  /**
   * Make a unique key from a file
   * @param file
   * @return
   */
  private static String makeKey(File file) {
    return file.getAbsolutePath();
  }
  //
  //other functions
  //
  /**
   * Function to construct the file used by the montagesize command
   * @param directory
   * @param datasetName
   * @param axisID
   * @return
   */
  private static File makeFile(String directory, String datasetName,
      AxisID axisID) {
    return new File(directory, datasetName + axisID.getExtension()
        + fileExtension);
  }
  /**
   * construct a piece list file
   * @return
   */
  private File makePieceListFile() {
    String filePath = file.getAbsolutePath();
    int extensionIndex = filePath.lastIndexOf(fileExtension);
    if (extensionIndex == -1) {
      throw new IllegalStateException("bad file name: file="
          + file.getAbsolutePath());
    }
    return new File(filePath.substring(0, extensionIndex) + ".pl");
  }

  /**
   * reset results
   *
   */
  private void reset() {
    fileExists = false;
    x.reset();
    y.reset();
    z.reset();
  }
  /**
   * run montagesize on the file
   * @throws IOException
   * @throws InvalidParameterException
   */
  public synchronized void read() throws IOException, InvalidParameterException {
    if (file == null) {
      throw new IOException("No stack specified.");
    }
    if (!file.exists()) {
      reset();
      return;
    }
    fileExists = true;
    //If the file hasn't been modified, don't reread
    if (!modifiedFlag.isModifiedSinceLastRead()) {
      return;
    }
    //put first timestamp after decide to read
    Utilities.timestamp("read", "montagesize", file, 0);
    //Run the montagesize command on the file.
    File pieceListFile = makePieceListFile();
    String[] commandArray;
    if (pieceListFile.exists()) {
      commandArray = new String[3];
    }
    else {
      commandArray = new String[2];
    }
    commandArray[0] = ApplicationManager.getIMODBinPath() + "montagesize";
    commandArray[1] = file.getAbsolutePath();
    if (pieceListFile.exists()) {
      commandArray[2] = pieceListFile.getAbsolutePath();
    }
    SystemProgram montagesize = new SystemProgram(commandArray, axisID);
    montagesize.setDebug(EtomoDirector.getInstance().isDebug());
    modifiedFlag.setReadingNow();
    montagesize.run();

    if (montagesize.getExitValue() != 0) {
      String[] stdOutput = montagesize.getStdOutput();
      if (stdOutput.length > 0) {
        ArrayList errorList = SystemProgram.parseError(stdOutput);
        if (errorList.size() > 0) {
          String message = "montagesize returned an error:\n";
          for (int i = 0; i < errorList.size(); i++) {
            message = message + errorList.get(i) + "\n";
          }
          Utilities.timestamp("read", "montagesize", file, -1);
          throw new InvalidParameterException(message);
        }
      }
    }
    // Throw an exception if the file can not be read
    String[] stdError = montagesize.getStdError();
    if (stdError.length > 0) {
      String message = "montagesize returned an error:\n";
      for (int i = 0; i < stdError.length; i++) {
        message = message + stdError[i] + "\n";
      }
      Utilities.timestamp("read", "montagesize", file, -1);
      throw new InvalidParameterException(message);
    }

    // Parse the output
    String[] stdOutput = montagesize.getStdOutput();
    if (stdOutput.length < 1) {
      Utilities.timestamp("read", "montagesize", file, -1);
      throw new IOException("montagesize returned no data");
    }

    for (int i = 0; i < stdOutput.length; i++) {
      //  Parse the size of the data
      //  Note the initial space in the string below
      String outputLine = stdOutput[i].trim();
      if (outputLine.startsWith("Total NX, NY, NZ:")) {
        String[] tokens = outputLine.split("\\s+");
        if (tokens.length < 7) {
          Utilities.timestamp("read", "montagesize", file, -1);
          throw new IOException(
              "Montagesize returned less than three parameters for image size");
        }
        x.set(tokens[4]);
        y.set(tokens[5]);
        z.set(tokens[6]);
        if (!x.isValid() || x.isNull()) {
          Utilities.timestamp("read", "montagesize", file, -1);
          throw new NumberFormatException("NX is not set, token is "
              + tokens[4] + "\n" + x.getInvalidReason());
        }
        if (!y.isValid() || y.isNull()) {
          Utilities.timestamp("read", "montagesize", file, -1);
          throw new NumberFormatException("NY is not set, token is "
              + tokens[5] + "\n" + y.getInvalidReason());
        }
        if (!z.isValid() || z.isNull()) {
          Utilities.timestamp("read", "montagesize", file, -1);
          throw new NumberFormatException("NZ is not set, token is "
              + tokens[6] + "\n" + z.getInvalidReason());
        }
      }
    }
    Utilities.timestamp("read", "montagesize", file, 1);
  }
  /**
   * 
   * @return
   */
  public ConstEtomoNumber getX() {
    return x;
  }
  /**
   * 
   * @return
   */
  public ConstEtomoNumber getY() {
    return y;
  }
  /**
   * 
   * @return
   */
  public ConstEtomoNumber getZ() {
    return z;
  }
  /**
   * 
   * @return
   */
  public boolean isFileExists() {
    return fileExists;
  } 
  //
  //self test functions
  //
  void selfTestInvariants() {
    if(!Utilities.isSelfTest()) {
      return;
    }
    if (instances == null) {
      throw new IllegalStateException("instances is null");
    }
    if (file == null) {
      throw new IllegalStateException("file is null");
    }
    String key = makeKey(file);
    if (key == null || key.matches("\\s*")) {
      throw new IllegalStateException("unable to make key: file=" + file);
    }
    Montagesize montagesize = (Montagesize) instances.get(key);
    if (montagesize == null) {
      throw new IllegalStateException("this instance is not in instances: key="+key);
    }
  }
}
/**
 * <p> $Log$
 * <p> Revision 1.4  2005/06/17 20:04:38  sueh
 * <p> bug# 685 Added timestamps to read().
 * <p>
 * <p> Revision 1.3  2005/04/25 21:42:49  sueh
 * <p> bug# 615 Passing the axis where a command originates to the message
 * <p> functions so that the message will be popped up in the correct window.
 * <p> This requires adding AxisID to many objects.
 * <p>
 * <p> Revision 1.2  2005/03/29 19:57:25  sueh
 * <p> bug# 623 Added fileExists state.  Added pieceListFile to the command
 * <p> string, if it exists.
 * <p>
 * <p> Revision 1.1  2005/03/08 02:01:47  sueh
 * <p> bug# 533 Object to call montagesize.
 * <p> </p>
 */