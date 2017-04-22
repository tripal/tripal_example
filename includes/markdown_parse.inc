<?php
/**
 * @ file
 * This file contains functions to allow templates to be written in
 * GitHub-flavoured mardown rather then straight HTML.
 *
 * Specifically it uses the JS library ShowDown (https://github.com/showdownjs/showdown) 
 * to parse the template file contents after the fact and replace them with HTML.
 */

/**
 * Menu Callback to render any Markdown file.
 *
 * Example of how to use this function:
 * @code

 * @endcode
 */
function trp_render_as_markdown($markdown_file) {

  // Add Javascript to convert markdown to html.
  drupal_add_js('https://cdn.rawgit.com/showdownjs/showdown/1.6.3/dist/showdown.min.js');

  // Add our script to call ShowDown on the content.
  drupal_add_js(drupal_get_path('module', 'trpexp_fields') . '/../includes/callParseMarkdown.js');

  // Render the content Markdown but embed within our convert tag.
  // We use the script tag to make sure html is not escaped at this point.
  $output = '<script class="markdown-content" type="text/template">';
  $output .= file_get_contents($markdown_file);
  $output .= '</script>';

  return $output;

}
