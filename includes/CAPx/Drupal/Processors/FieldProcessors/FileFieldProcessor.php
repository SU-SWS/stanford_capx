<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * File processor should have an array of data passed in.
 *
 */

namespace CAPx\Drupal\Processors\FieldProcessors;
use CAPx\APILib\HTTPClient;

class FileFieldProcessor extends FieldProcessor {

  // The path to save the file to.
  protected $saveDir = "public://capx/profile-images/";

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

    // Request the file
    $response = $this->fetchRemoteFile($data);

    // Save the file.
    $filename = $this->getFileName($data);
    $file = file_save_data($response->getBody(), $filename, FILE_EXISTS_REPLACE);

    if (!$file) {
      throw new \Exception("Could not save profile image: " . $data['url']);
    }

    // All went well go and save it.
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $entity->{$fieldName}->set(
      array(
        'fid' => $file->fid,
        'file' => $file,
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
      throw new \Exception("Could not fetch profile image: " . $data['url']);
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
    switch ($type) {
      case 'image/png':
        return ".png";
        break;

      case "image/jpg":
      case "image/jpeg":
      default:
        return ".jpg";
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
