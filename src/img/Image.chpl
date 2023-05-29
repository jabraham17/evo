/*
wrapper around C bindings for an img abstraction
also wraps C bindings to generate simple bmp files
*/
module Image {
  use CTypes;

  require "helper.h";
  require "helper.c";

  require "img.h";
  require "img.c";

  extern record img_pixel_t {
    var blue: uint(8);
    var green: uint(8);
    var red: uint(8);
  }
  extern record img_t {
    var pixels: c_ptr(img_pixel_t);
    var width: c_size_t;
    var height: c_size_t;
  }

  private extern proc img_create(width: c_size_t, height: c_size_t): c_ptr(img_t);
  private extern proc img_get_size(img: c_ptr(img_t)): c_size_t;
  private extern proc img_get_pixel(img: c_ptr,
                                col: c_size_t,
                                row: c_size_t): c_ptr(img_pixel_t);
  private extern proc img_set_bgr(img: c_ptr,
                              blue: uint(8),
                              green: uint(8),
                              red: uint(8),
                              col: c_size_t,
                              row: c_size_t);
  private extern proc img_destroy(img: c_ptr);

  require "bmp.h";
  require "bmp.c";

  extern record bmp_t {

  }

  private extern proc bmp_create_from_img(img: c_ptr(img_t)): c_ptr(bmp_t);
  private extern proc bmp_write_to_file(bmp: c_ptr(bmp_t), filename: c_string);
  private extern proc bmp_destroy(bmp: c_ptr(bmp_t));

  record image {
    var img_handle_: c_ptr(img_t);

    proc init(width:uint, height:uint) {
      this.img_handle_ = img_create(width:c_size_t, height:c_size_t);
    }
    proc deinit() {
      if this.img_handle_ {
        img_destroy(this.img_handle_);
        this.img_handle_ = nil;
      }
    }

    proc set(col: uint, row: uint, blue: uint(8), green: uint(8), red: uint(8)) {
      img_set_bgr(this.img_handle_, blue, green , red, col:c_size_t, row:c_size_t);
    }
    proc set(pos: 2*uint, bgr:3*uint(8)) {
      img_set_bgr(this.img_handle_, bgr[0], bgr[1] , bgr[2], pos[0]:c_size_t, pos[1]:c_size_t);
    }

    proc saveAsBMP(filename:string) {
      var bmp = bmp_create_from_img(this.img_handle_);
      bmp_write_to_file(bmp, filename.c_str());
      bmp_destroy(bmp);
    }

  }

}
