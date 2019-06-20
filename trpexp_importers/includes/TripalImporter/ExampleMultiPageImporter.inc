<?php

class ExampleMultiPageImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Example Multi-Page Importer';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_multipage_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Loads TST files through a multi-page form.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['txt', 'tst', 'csv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'TST is a fictional format.  Its a 2-column, CSV file.  The columns should be of the form featurename, and text';

  // --------------------------------------------------------------------------
  // The following are set to FALSE to allow us to place our own form
  // elements rather then use the default ones. This is needed to ensure each
  // field is only on one page rather then all of them ;-p
  // --------------------------------------------------------------------------

  /**
   * We need the file to not be required so there is no validation error on
   * non-file containing form pages.
   */
  public static $file_required = FALSE;

  /**
   * We set these to FALSE because otherwise they will be on every page
   * of the form. Instead we will add the file upload ourself in page 1.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => FALSE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  /**
   * We remove the analysis field as well for the same reason.
   */
  public static $use_analysis = FALSE;

  /**
   * We remove the button in order to control the label on it.
   */
  public static $use_button = FALSE;

  /**
   * @see TripalImporter::form().
   */
  public function form($form, &$form_state) {

    // If the form_state values are not set then we are on the first page.
    if (!isset($form_state['values'])) {
      $current_stage = 1;
    }
    // Otherwise, we are one page further then we were last time!
    else {
      $current_stage = $form_state['values']['stage'] + 1;
    }

    // Now save the current stage for use later.
    $form['stage'] = [
      '#type' => 'hidden',
      '#value' => $current_stage,
    ];

    $form = $this->save_stage_values($form, $form_state);

    // Based on the stage we are on, we need to pull the correct form.
    switch ($current_stage) {
      case 1:
        $form = $this->form_page1($form, $form_state);
        break;
      case 2:
        $form = $this->form_page2($form, $form_state);
        break;
      case 3:
        $form = $this->form_page3($form, $form_state);
        break;
    }

    // Add in navigation button.
    // We want it to be "Next" unless we are on the last page.
    $button_text = ($current_stage < 3) ? "Next" : "Import";
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $button_text,
    ];

    // The form function must always return our form array.
    return $form;
  }

  /**
   * This page will provide our analysis link.
   */
  public function form_page1($form, $form_state) {

    $form['stage_notice'] = [
      '#type' => 'markup',
      '#markup' => '<h1>STAGE 1</h1>',
    ];

    // get the list of analyses
    $sql = "SELECT * FROM {analysis} ORDER BY name";
    $org_rset = chado_query($sql);
    $analyses = [];
    $analyses[''] = '';
    while ($analysis = $org_rset->fetchObject()) {
      $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
    }
    $form['analysis_id'] = [
      '#title' => t('Analysis'),
      '#type' => t('select'),
      '#description' => t('Choose the analysis to which the uploaded data will be associated. ' .
        'Why specify an analysis for a data load?  All data comes from some place, even if ' .
        'downloaded from a website. By specifying analysis details for all data imports it ' .
        'provides provenance and helps end user to reproduce the data set if needed. At ' .
        'a minimum it indicates the source of the data.'),
      '#required' => TRUE,
      '#options' => $analyses,
    ];

    return $form;
  }

  /**
   * This page will provide our file upload.
   */
  public function form_page2($form, $form_state) {
    $class = get_called_class();
    global $user;

    $form['stage_notice'] = [
      '#type' => 'markup',
      '#markup' => '<h1>STAGE 2</h1>',
    ];

    // Now to add the file + description based on our class.
    $form['file'] = [
      '#type' => 'fieldset',
      '#title' => t($class::$upload_title),
      '#description' => t($class::$upload_description),
    ];

    $existing_files = tripal_get_user_uploads($user->uid, $class::$file_types);
    if (count($existing_files) > 0) {
      $fids = [0 => '--Select a file--'];
      foreach ($existing_files as $fid => $file) {
        $fids[$fid] = $file->filename . ' (' . tripal_format_bytes($file->filesize) . ') ';
      }
      $form['file']['file_upload_existing'] = [
        '#type' => 'select',
        '#title' => t('Existing Files'),
        '#description' => t('You may select a file that is already uploaded.'),
        '#options' => $fids,
      ];
    }
    $form['file']['file_upload'] = [
      '#type' => 'html5_file',
      '#title' => '',
      '#description' => 'Remember to click the "Upload" button below to send ' .
        'your file to the server.  This interface is capable of uploading very ' .
        'large files.  If you are disconnected you can return, reload the file and it ' .
        'will resume where it left off.  Once the file is uploaded the "Upload ' .
        'Progress" will indicate "Complete".  If the file is already present on the server ' .
        'then the status will quickly update to "Complete".',
      '#usage_type' => 'tripal_importer',
      '#usage_id' => 0,
      '#allowed_types' => $class::$file_types,
      '#cardinality' => $class::$cardinality,
    ];

    return $form;
  }

  /**
   * This page will provide our cvterm field.
   */
  public function form_page3($form, $form_state) {

    $form['stage_notice'] = [
      '#type' => 'markup',
      '#markup' => '<h1>STAGE 3</h1>',
    ];

    // For our example loader let's assume that there is a small list of
    // vocabulary terms that are appropriate as properties for the genomics
    // features. Therefore, we will provide an array of sequence ontology terms
    // the user can select from.
    $terms = [
      ['id' => 'SO:0000235'],
      ['id' => 'SO:0000238'],
      ['id' => 'SO:0000248']
    ];

    // Construct the options for the select drop down.
    $options = [];
    // Iterate through the terms array and get the term id and name using
    // appropriate Tripal API functions.
    foreach ($terms as $term){
      $term_object = chado_get_cvterm($term);
      $id = $term_object->cvterm_id;
      $options[$id] = $term_object->name;
    }

    // Provide the Drupal Form API array for a select box.
    $form['pick_cvterm'] =  [
      '#title' => 'CVterm',
      '#description' => 'Please pick a CVterm.  The loaded TST file will associate the values with this term as a feature property.',
      '#type' => 'select',
      '#default_value' => '0',
      '#options' => $options,
      '#empty_option' => '--please select an option--'
    ];

    return $form;
  }

  /**
   * This function will ensure values from other steps are saved through to
   * the last step. This allows our TripalImporter::run() to act as through
   * it's a single page form!
   */
  public function save_stage_values($form, $form_state) {

    if (isset($form_state['values'])) {

      // STAGE 1: analysis_id.
      if (isset($form_state['values']['analysis_id'])) {
        $form['analysis_id'] = [
          '#type' => 'hidden',
          '#value' => $form_state['values']['analysis_id'],
        ];
      }

      // STAGE 2: fid.
      if (isset($form_state['values']['fid'])) {
        $form['fid'] = [
          '#type' => 'hidden',
          '#value' => $form_state['values']['fid'],
        ];

        // We also want to look up the filepath and save that as well.
        if (!isset($form_state['values']['files'])) {
          $fid = $form_state['values']['fid'];

          $form['files'] = [
            '#tree' => TRUE,
          ];

          // Handle multiple file uploads.
          if (preg_match('/\|/', $fid)) {
            $fids = explode('|', $fid);
            foreach ($fids as $id) {
              $file = file_load($id);
              $form['files'][] = [
                'file_path' => [
                  '#type' => 'hidden',
                  '#value' => drupal_realpath($file->uri),
                ],
                'fid' => [
                  '#type' => 'hidden',
                  '#value' => $id,
                ]
              ];
            }
          }
          // Handle a single file.
          else {
            $file = file_load($fid);
            $form['files'][] = [
              'file_path' => [
                '#type' => 'hidden',
                '#value' => drupal_realpath($file->uri),
              ],
              'fid' => [
                '#type' => 'hidden',
                '#value' => $fid,
              ]
            ];
          }
        }
      }

      // STAGE 3: pick_cvterm.
      if (isset($form_state['values']['pick_cvterm'])) {
        $form['pick_cvterm'] = [
          '#type' => 'hidden',
          '#value' => $form_state['values']['pick_cvterm'],
        ];
      }
    }

    return $form;
  }

  /**
   * @see TripalImporter::formValidate().
   */
  public function formValidate($form, &$form_state) {

    // Always call the TripalImporter (i.e. parent) formValidate as it provides
    // some important feature needed to make the form work properly.
    parent::formValidate($form, $form_state);

    // If we are on the page with the file upload (page 2)...
    if (isset($form['file']) AND isset($form['file']['file_upload'])) {
      // then check we have a file.
      // First check the upload element.
      if (empty($form_state['values']['file_upload'])) {
        // Then check the existing file element.
        if (empty($form_state['values']['file_upload_existing'])) {
          form_set_error('file', 'You must provide a file.');
        }
        else {
          $form_state['values']['fid'] = $form_state['values']['file_upload_existing'];
        }
      }
      else {
        $form_state['values']['fid'] = $form_state['values']['file_upload'];
      }
    }

    // If we are on the page with the cvterm (page 3)...
    if (isset($form['pick_cvterm'])) {
      // Get the chosen CV term form the form state and if there is no value
      // set warn the user.
      $chosen_cvterm = $form_state['values']['pick_cvterm'];
      if ($chosen_cvterm == 0) {
        form_set_error('pick_cvterm', 'Please choose a CVterm.');
      }
    }
  }

  /**
   * @see TripalImporter::formSubmit().
   */
  public function formSubmit($form, &$form_state) {
    $stage = $form_state['values']['stage'];
    // If we're not done yet, then make sure the form rebuilds.
    if ($stage < 3) {
      $form_state['rebuild'] = TRUE;
    }
    else {
      $from_state['rebuild'] = FALSE;
    }
  }

  /**
   * @see TripalImporter::run().
   */
  public function run() {

    // All values provided by the user in the Importer's form widgets are
    // made available to us here by the Class' arguments member variable.
    $arguments = $this->arguments['run_args'];

    // The path to the uploaded file is always made available using the
    // 'files' argument. The importer can support multiple files, therefore
    // this is an array of files, where each has a 'file_path' key specifying
    // where the file is located on the server.
    // NOTE: this is in the run_args because we made our own file_upload.
    $file_path = $this->arguments['run_args']['files'][0]['file_path'];

    // The analysis that the data being imported is associated with is always
    // provided as an argument.
    $analysis_id = $arguments['analysis_id'];

    // Any of the widgets on our form are also available as an argument.
    $cvterm_id = $arguments['pick_cvterm'];

    // Now that we have our file path, analysis_id and CV term we can load
    // the file.  We'll do so by creating a new function in our class
    // called "loadMyFile" and pass these arguments to it.
    $this->loadMyFile($analysis_id, $file_path, $cvterm_id);

  }

  /**
   * This is verbatim from ExampleImporter. Nothing needs to change!
   */
  public function loadMyFile($analysis_id, $file_path, $cvterm_id){

    // We want to provide a progress report to the end-user so that they:
    // 1) Recognize that the loader is not hung if running a large file, but is
    //    executing
    // 2) Provides some indicatation for how long the file will take to load.
    //
    // Here we'll get the size of the file and tell the TripalImporter how
    // many "items" we have to process (in this case bytes of the file).
    $filesize = filesize($file_path);
    $this->setTotalItems($filesize);
    $this->setItemsHandled(0);

    // Loop through each line of file.  We use the fgets function so as not
    // to load the entire file into memory but rather to iterate over each
    // line separately.
    $bytes_read = 0;
    $in_fh = fopen($file_path, "r");
    while ($line = fgets($in_fh)) {

      // Calculate how many bytes we have read from the file and let the
      // importer know how many have been processed so it can provide a
      // progress indicator.
      $bytes_read += drupal_strlen($line);
      $this->setItemsHandled($bytes_read);

      // Remove any trailing white-space from the line.
      $line = trim($line);

      // Split line on a comma into an array.  The feature name appears in the
      // first "column" of data and the property in the second.
      $cols = explode(",", $line);
      $feature_name = $cols[0];
      $this_value = $cols[1];

      // Our file has a header with the name 'Feature name' expected as the
      // title for the first column. If we see this ignore it.
      if ($feature_name == 'Feature name'){
         continue;
      }

      // Using the name of the feature from the file, see if we can find a
      // record in the feature table of Chado that matches.  Note: in reality
      // the feature table of Chado has a unique contraint on the uniquename,
      // organism_id and type_id columns of the feature table.  So, to ensure
      // we find a single record ideally we should include the organism_id and
      // type_id in our filter and that would require more widgets on our form!
      // For simplicity, we will just search on the uniquename and hope we
      // find unique features.
      $match = ['uniquename' => $feature_name];
      $results = chado_select_record('feature', ['feature_id'], $match);

      // The chado_select_record function always returns an array of matches. If
      // we found no matches then this feature doesn't exist and we'll skip
      // this line of the file.  But, log this issue so the user knows about it.
      if (count($results) == 0) {
        $this->logMessage('The feature, !feature, does not exist in the database',
          ['!feature' => $feature_name], TRIPAL_WARNING);
        continue;
      }

      // If we failed to find a unique feature then we should warn the user
      // but keep on going.
      if (count($results) == 0) {
        $this->logMessage('The feature, !feature, exists multiple times. ' .
          'Cannot add a property', ['!feature' => $feature_name], TRIPAL_WARNING);
        continue;
      }

      // If we've made it this far then we have a feature and we can do the
      // insert.
      $feature = $results[0];
      $record = [
        'table' => 'feature',
        'id' => $feature->feature_id
      ];
      $property = [
        'type_id' => $cvterm_id,
        'value' => $this_value,
      ];
      $options = ['update_if_present' => TRUE];
      chado_insert_property($record, $property, $options);
    }
  }
}
