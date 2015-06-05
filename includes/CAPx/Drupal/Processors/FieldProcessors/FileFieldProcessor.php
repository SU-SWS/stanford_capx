<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class FileFieldProcessor extends FieldTypeProcessor {

  /**
   * Override the default put implementation because file needs special things.
   *
   * The file put needs an array of information per entry. This is ok because
   * the CAP API provides it in a way we can use.
   *
   * @param array $data
   *   Fetched remote data. There is two required entries in this array:
   * - contentType - Describing file type
   * - url - URL to fetch file
   *
   * @see FieldProcessorAbstract::put()
   */
  public function put($data) {
    $data = $this->process($data);
    parent::put($data);
  }

  /**
   * Process incoming data.
   *
   * The meat function of this processor. Take the data and turn it into a file.
   * This function will fetch the remote file and save it to the file system.
   *
   * @param array $data
   *   An array of information from the CAP API.
   *
   * @return array
   *   Data that feets to Drupal.
   */
  public function process($data) {
    $return = array();
    // Normalize data because it comes in a bit funky as we take whole array
    // from the CAP API data.
    $data = reset($data);
    if (!is_array($data)) {
      $this->logIssue(new \Exception(t('FileFieldProcessor Requires Data to be an array')));
      return array();
    }

    foreach ($data as $value) {
      // Validate we have required data.
      if (empty($value['contentType']) || empty($value['url'])) {
        $this->logIssue(new \Exception(t('Missing required information for field processor.')));
        continue;
      }

      // If the placeholder variable is available check so we don't get an
      // empty image.
      if (isset($value['placeholder']) && $value['placeholder'] === TRUE) {
        continue;
      }

      // Allow altering as this could get messy.
      drupal_alter('capx_pre_fetch_remote_file', $value);

      // @todo We can put a check in place to see if file was changed so
      // we don't fetch it again, but Drupal doesn't allow to store
      // lastModified data by default, this means we will need to handle
      // this ourselves. DO we really want to do it?
      // Request the file from the remote server.
      $file_data = $this->fetchRemoteFile($value);
      if (empty($file_data)) {
        continue;
      }

      $filename = $this->getFilePath($value);
      if (!$filename) {
        continue;
      }

      $file = file_save_data($file_data, $filename, FILE_EXISTS_REPLACE);

      if (!$file) {
        $this->logIssue(new \Exception(t('Could not save file with filename %name from URL @url', array('%name' => $filename, '@url' => $value['url']))));
        continue;
      }

      // We have a file, allow more altering.
      drupal_alter('capx_post_save_remote_file', $file, $filename);

      // @todo This Place is good candidate to become a new method, which can be overridden in image.
      $fieldName = $this->getFieldName();
      $fieldInfo = field_info_field($fieldName);
      $entityType = $this->getEntity()->type();
      $bundle = $this->getEntity()->getBundle();
      $fieldInstance = field_info_instance($entityType, $fieldName, $bundle);

      if ($fieldInfo['type'] == 'file') {
        $display = TRUE;
        if (!empty($fieldInfo['settings']['display_field'])) {
          $display = (bool) $fieldInfo['settings']['display_default'];
        }

        $description = '';
        if (!empty($fieldInstance['settings']['description_field']) && isset($value['label']) && !empty($value['label']['text'])) {
          $description = $value['label']['text'];
        }

        $return['description'][] = $description;
        $return['display'][] = $display;
      }

      $return['fid'][] = $file->fid;
    }

    return $return;
  }

  /**
   * Fetches a remote file from the CAP API servers.
   *
   * @param array $data
   *   An array of information needed to fetch a file from
   * the CAP API servers
   *
   * @return string
   *   Retrieved file data.
   */
  public function fetchRemoteFile($data) {
    $file = NULL;
    // Fetch the image from CAP.
    $client = new \CAPx\APILib\HTTPClient();
    $guzzle = $client->getHttpClient();
    try {
      $response = $guzzle->get($data['url'])->send();
    }
    catch (\Exception $e) {
      $this->logIssue($e);
    }

    if ($response->getStatusCode() !== 200) {
      $this->logIssue(new \Exception(t('Could not fetch file from URL: @url', array('@url' => $data['url']))));
    }
    else {
      $file = $response->getBody(TRUE);
    }

    return $file;
  }

  /**
   * Gets the file name for the remote file.
   *
   * Appending the appropriate file extension to it.
   *
   * @param array $data
   *   File data.
   *
   * @return string
   *   A unique filename.
   */
  public function getFileName($data) {

    $salt = time();

    $extension = $this->getExtentionByType($data['contentType']);
    $filename = md5($data['url'] . $salt) . $extension;

    return $filename;
  }


  /**
   * Returns file extension based on MIME type.
   *
   * The file type is provided by the CAP api but a file extension is not. Here
   * we match them up.
   *
   * @param string $type
   *   The type of file being saved.
   *
   * @return string
   *   The matching extension with leading period.
   */
  public function getExtentionByType($type) {
    include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
    $mapping = file_mimetype_mapping();
    $mimetypes = array_flip($mapping['mimetypes']);
    $extensions = array_flip($mapping['extensions']);

    $extension = isset($mimetypes[$type]) ? $extensions[$mimetypes[$type]] : 'unknown';

    return '.' . $extension;
  }

  /**
   * Returns directory to store current file.
   *
   * @return string
   *   The destination to save the file.
   */
  public function getSaveDir() {
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $entityType = $this->getEntity()->type();
    $bundle = $this->getEntity()->getBundle();
    $fieldInstance = field_info_instance($entityType, $fieldName, $bundle);

    $schema = $fieldInfo['settings']['uri_scheme'] . '://';
    $fileDirectory = $fieldInstance['settings']['file_directory'];
    $fileDirectory = empty($fileDirectory) ? $schema : $schema . $fileDirectory . '/';

    if (!file_prepare_directory($fileDirectory, FILE_CREATE_DIRECTORY)) {
      $this->logIssue(new \Exception(t('Cannot create directory %directory to store fetched file.', array('%directory' => $fileDirectory))));
      $fileDirectory = NULL;
    }

    return $fileDirectory;
  }

  /**
   * Returns file path.
   *
   * Includes access scheme and filename.
   *
   * @param array $data
   *   File data.
   *
   * @return string
   *   File path.
   */
  public function getFilePath($data) {
    return $this->getSaveDir() ? $this->getSaveDir() . $this->getFileName($data) : NULL;
  }

}
