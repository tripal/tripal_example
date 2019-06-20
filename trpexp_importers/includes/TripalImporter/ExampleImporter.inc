<?php

class ExampleImporter extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Example TST File Importer';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_tst_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Loads TST files';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['txt', 'tst', 'csv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'TST is a fictional format.  Its a 2-column, CSV file.  The columns should be of the form featurename, and text';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

  public function form($form, &$form_state) {


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

    // The form function must always return our form array.
    return $form;
  }

  public function formValidate($form, &$form_state) {

    // Always call the TripalImporter (i.e. parent) formValidate as it provides
    // some important feature needed to make the form work properly.
    parent::formValidate($form, $form_state);

    // Get the chosen CV term form the form state and if there is no value
    // set warn the user.
    $chosen_cvterm = $form_state['values']['pick_cvterm'];
    if ($chosen_cvterm == 0) {
      form_set_error('pick_cvterm', 'Please choose a CVterm.');
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    // All values provided by the user in the Importer's form widgets are
    // made available to us here by the Class' arguments member variable.
    $arguments = $this->arguments['run_args'];

    // The path to the uploaded file is always made available using the
    // 'files' argument. The importer can support multiple files, therefore
    // this is an array of files, where each has a 'file_path' key specifying
    // where the file is located on the server.
    $file_path = $this->arguments['files'][0]['file_path'];

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
