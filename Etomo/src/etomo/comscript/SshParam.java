package etomo.comscript;

import java.util.ArrayList;
import java.util.List;

import etomo.process.SystemProgram;
import etomo.type.AxisID;
import etomo.type.EtomoBoolean2;
import etomo.type.EtomoVersion;

/**
 * <p>Description: </p>
 * 
 * <p>Copyright: Copyright 2008</p>
 *
 * <p>Organization:
 * Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEMC),
 * University of Colorado</p>
 * 
 * @author $Author$
 * 
 * @version $Revision$
 * 
 * <p> $Log$ </p>
 */
final class SshParam {
  public static final String rcsid = "$Id$";

  static final SshParam INSTANCE = new SshParam();

  private EtomoBoolean2 timeoutAvailable = null;

  final List getCommand(boolean useTimeoutIfPossible, String computer) {
    List command = new ArrayList();
    command.add("ssh");
    //prevents ssh from waiting for an answer when connecting to a computer for
    //the first time
    //see man ssh_config
    command.add("-x");
    command.add("-o");
    command.add("StrictHostKeyChecking=no");
    if (useTimeoutIfPossible && isTimeoutAvailable()) {
      //Timeout doesn't work with older versions of Redhat (see bug# 1043).
      //maximum connection timeout for a down computer
      command.add("-o");
      command.add("ConnectTimeout=5");
    }
    command.add("-o");
    //prevents password prompts when the publickey authentication fails
    command.add("PreferredAuthentications=publickey");
    command.add("-v");
    command.add(computer);
    return command;
  }

  /**
   * Sets timeoutAvailable based on the result of running "ssh -v".  Only sets
   * timeoutAvailable once.  If running ssh -v fails, timeoutAvailable is set to
   * false.
   * @return timeoutAvailable
   */
  synchronized boolean isTimeoutAvailable() {
    if (timeoutAvailable != null) {
      return timeoutAvailable.is();
    }
    //Set timeoutAvailable from "ssh -v".  OpenSSH with a version of 3.9 or
    //greater will understand the ConnectTimeout option.
    timeoutAvailable = new EtomoBoolean2();
    //Run ssh -v.
    SystemProgram systemProgram = new SystemProgram(System
        .getProperty("user.dir"), new String[] { "ssh", "-v" }, AxisID.ONLY,
        null);
    systemProgram.run();
    //Find and parse the OpenSSH version.
    String[] stderr = systemProgram.getStdError();
    if (stderr != null && stderr.length > 0) {
      String appString = "openssh";
      int i = 0;
      while (stderr[i].toLowerCase().indexOf(appString) == -1) {
        i++;
      }
      if (i < stderr.length) {
        //Find and store the version of OpenSSH (OpenSSH_version, ...).
        String[] versionInfoArray = stderr[i].toLowerCase().trim().split(
            "[_,\\s]+");
        if (versionInfoArray != null && versionInfoArray.length > 0) {
          i = 0;
          while (versionInfoArray[i++].toLowerCase().indexOf(appString) == -1) {
          }
          if (i < versionInfoArray.length) {
            EtomoVersion openSshVersion = EtomoVersion
                .getDefaultInstance(versionInfoArray[i]);
            if (openSshVersion.ge(EtomoVersion.getDefaultInstance("3.9"))) {
              timeoutAvailable.set(true);
              return true;
            }
          }
        }
      }
    }
    timeoutAvailable.set(false);
    return false;
  }
}
