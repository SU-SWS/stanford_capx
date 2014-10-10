<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class FileFieldProcessor extends FieldTypeProcessor {

  // The path to save the file to.
  // @todo: Make this a Drupal configuration setting.
  protected $saveDir = "public://capx/profile-files/";

  /**
   * Override the default put implementation because file needs special things.
   * The file put needs an array of information per entry. This is ok because
   * the CAP API provides it in a way we can use.
   * @param   array $data [description]
   *                $data['contentType']  = "image/jpeg"
   *                $data['placeholder']  = false
   *                $data['type']         = "big"
   *                $data['url']          = "https://...."
   */
  public function put($data) {

    // Normalize data because it comes in a bit funky as we take whole array
    // from the CAP API data.
    $data = $data[0];

    if (!is_array($data)) {
      throw new \Exception("FileFieldProcessor Requires Data to be an array");
    }

    // Loop through each and ensure that the values are there.
    foreach ($data as $index => $values) {
      // Validate we have our data
      if (!isset($values['contentType']) || !isset($values['url'])) {
        throw new \Exception("Missing required information for field processor.");
      }

      // Ok, enough validation lets make bacon.
      $this->process($values);
    }

  }

  /**
   * The meat function of this processor. Take the data and turn it into a file.
   * This function will fetch the remote file and save it to the file system.
   * @todo : Put a check in place to see if the file has changed. Waiting on the
   * cap api to provide a 'last updated' timestamp on the file itself.
   * @param  array $data any array of information from the CAP API
   * @return [type]         [description]
   */
  public function process($data) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);

    // Allow altering as this could get messy.
    drupal_alter('capx_pre_fetch_remote_file', $data);

    // Request the file from the remote server.
    $response = $this->fetchRemoteFile($data);

    // Save the file.
    $filename = $this->getFileName($data);
    $body = $response->getBody();
    $file = file_save_data($body, $filename, FILE_EXISTS_REPLACE);

    if (!$file) {
      watchdog('stanford_capx', 'Could not save file: ' . $data['url'], array(), WATCHDOG_DEBUG);
      return;
    }

    // We have a file, allow more altering.
    drupal_alter('capx_post_save_remote_file', $file, $filename);

    // All went well go and save it.
    $setData = array(
      'fid' => $file->fid,
      'file' => $file,
      'display' => variable_get('capx_display_file_default', 1),
    );

    // Check cardinality.
    if ($fieldInfo['cardinality'] !== "1") {
      // wrap the setData in another array.
      $setData = array($setData);
    }

    // Set the thing.
    $entity->{$fieldName}->set($setData);
  }

  /**
   * Fetches a remote file from the CAP API servers.
   * @param  array $data an array of information needed to fetch a file from
   * the CAP API servers
   * @return Response         Guzzle response object
   */
  public function fetchRemoteFile($data) {
    // Fetch the image from CAP.
    $client = new \CAPx\APILib\HTTPClient();
    $guzzle = $client->getHttpClient();
    $response = $guzzle->get($data['url'])->send();

    if ($response->getStatusCode() !== 200) {
      throw new \Exception("Could not fetch file: " . $data['url']);
    }

    return $response;
  }

  /**
   * Gets the file name for the remote file by appending the appropriate
   * file extension to it.
   * @param  [type] $data [description]
   * @return string       a unique filename
   */
  public function getFileName($data) {

    $saveDir = $this->getSaveDir();
    $extension = $this->getExtentionByType($data['contentType']);
    file_prepare_directory($saveDir, FILE_CREATE_DIRECTORY);
    $filename = $saveDir . md5($data['url']) . $extension;

    return $filename;
  }


  /**
   * The file type is provided by the CAP api but a file extention is not. Here
   * we match them up.
   * @param  string $type the type of file being saved.
   * @return string       the matching extension with leading period.
   */
  public function getExtentionByType($type) {

    if (empty($type)) {
      return false;
    }
     switch ($type) {
       case 'image/bmp': return '.bmp';
       case 'image/gif': return '.gif';
       case 'image/jpeg': return '.jpg';
       case 'image/tiff': return '.tif';
       case 'image/x-icon': return '.ico';
       case 'image/x-rgb': return '.rgb';
       case 'image/png': return '.png';
       case 'image/x-jps': return '.jps';
       default: return false;
      // TODO: https://github.com/EllisLab/CodeIgniter/blob/develop/application/config/mimes.php
     }

  }

  /**
   * Getter function
   * @return string the destination to save the file. PUBLIC.
   */
  public function getSaveDir() {
    return $this->saveDir;
  }

  /**
   * Setter function
   * @param string $dir the destination to save the file. PUBLIC.
   */
  public function setSaveDir($dir) {
    $this->saveDir = $dir;
  }


}
