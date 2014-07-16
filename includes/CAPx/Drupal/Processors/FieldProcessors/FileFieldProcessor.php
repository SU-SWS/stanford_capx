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
   * [put description]
   * @param  [type] $data [description]
   *                $data['contentType']  = "image/jpeg"
   *                $data['placeholder']  = false
   *                $data['type']         = "big"
   *                $data['url']          = "https://...."
   * @return [type]       [description]
   */
  public function put($data) {

    // Normalize data because it comes in a bit funky as we take whole array
    $data = $data[0];

    if (!is_array($data)) {
      throw new \Exception("FileFieldProcessor Requires Data to be an array");
    }

    foreach ($data as $index => $values) {
      // Validate we have our data
      if (!isset($values['contentType']) || !isset($values['url'])) {
        throw new \Exception("Missing required information for field processor.");
      }
      $this->process($values);
    }

  }

  /**
   * [process description]
   * @param  [type] $values [description]
   * @return [type]         [description]
   */
  public function process($data) {

    drupal_alter('capx_pre_fetch_remote_file', $data);

    // Request the file.
    $response = $this->fetchRemoteFile($data);

    // Save the file.
    $filename = $this->getFileName($data);
    $file = file_save_data($response->getBody(), $filename, FILE_EXISTS_REPLACE);

    if (!$file) {
      throw new \Exception("Could not save file: " . $data['url']);
    }

    drupal_alter('capx_post_save_remote_file', $file, $filename);

    // All went well go and save it.
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $entity->{$fieldName}->set(
      array(
        'fid' => $file->fid,
        'file' => $file,
        'display' => variable_get('capx_display_file_default', 1),
      )
    );

  }

  /**
   * [fetchRemoteFile description]
   * @param  [type] $values [description]
   * @return [type]         [description]
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
   * [getFileName description]
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function getFileName($data) {

    $saveDir = $this->getSaveDir();
    $extension = $this->getExtentionByType($data['contentType']);
    file_prepare_directory($saveDir, FILE_CREATE_DIRECTORY);
    $filename = $saveDir . md5($data['url']) . $extension;

    return $filename;
  }


  /**
   * [getExtentionByType description]
   * @param  [type] $type [description]
   * @return [type]       [description]
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
   * [getSaveDir description]
   * @return [type] [description]
   */
  public function getSaveDir() {
    return $this->saveDir;
  }

  /**
   * [setSaveDir description]
   * @param [type] $dir [description]
   */
  public function setSaveDir($dir) {
    $this->saveDir = $dir;
  }


}
