/* Copyright (c) 2008-2017 the MRtrix3 contributors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * MRtrix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * For more details, see http://www.mrtrix.org/.
 */


#include <sstream>

#include "command.h"
#include "image.h"
#include "transform.h"
#include "phase_encoding.h"
#include "interp/linear.h"


using namespace MR;
using namespace App;


void usage ()
{
  AUTHOR = "Daan Christiaens (daan.christiaens@kcl.ac.uk)";

  SYNOPSIS = "Evaluate the amplitude of a 5-D image of multi-shell "
             "spherical harmonic functions along specified directions.";

  ARGUMENTS
    + Argument ("input",
                "the input image.").type_image_in ()
    + Argument ("field",
                "the B0 field.").type_file_in ()
    + Argument ("output",
                "the output, field unwrapped, image.").type_image_out ();

  OPTIONS
    + Option ("transform",
              "rigid transformation, applied to the field.")
      + Argument("T").type_file_in()

    + PhaseEncoding::ImportOptions

    + DataType::options();
}


using value_type = float;


class FieldUnwrap {
  MEMALIGN(FieldUnwrap)
  public:

    FieldUnwrap (const Image<value_type>& data, const Image<value_type>& field,
                 const Eigen::MatrixXd& petable, const transform_type& T) :
      dinterp (data, 0.0f),
      finterp (field, 0.0f),
      PE (petable.leftCols<3>()),
      Tf (Transform(field).scanner2voxel * T)
    {
      PE.array().colwise() *= petable.col(3).array();
    }

    void operator() (Image<value_type>& out)
    {
      Eigen::Vector3 vox, pos;
      assign_pos_of(out).to(vox);
      for (size_t v = 0; v < out.size(3); v++) {
        pos = vox;
        double b0, b1 = 0;
        for (size_t j = 0; j < 5; j++) {
          finterp.voxel(Tf * pos);
          b0 = finterp.value();
          if (b0 == 0) break;
          pos = vox - b0 * PE.block<1,3>(v, 0).transpose();
          if (std::abs(b1-b0) < 0.25f) break;
          b1 = b0;
        }
        dinterp.index(3) = out.index(3) = v;
        dinterp.voxel(pos);
        out.value() = dinterp.value();
      }
    }

  private:
    Interp::Linear<Image<value_type>> dinterp;
    Interp::Linear<Image<value_type>> finterp;
    Eigen::Matrix<double, Eigen::Dynamic, 3> PE;
    transform_type Tf;
};


void run ()
{
  auto data = Image<value_type>::open(argument[0]);
  auto field = Image<value_type>::open(argument[1]);

  auto petable = PhaseEncoding::get_scheme(data);
  // -----------------------  // TODO: Eddy uses a reverse LR axis for storing
  petable.col(0) *= -1;       // the PE table, akin to the gradient table.
  // -----------------------  // Fix in the eddy import/export functions in core.

  // Apply rigid rotation to field.
  transform_type T;
  T.setIdentity();
  auto opt = get_options("transform");
  if (opt.size())
    T = load_transform(opt[0][0]);
  T = T * Transform(data).voxel2scanner;

  // Save output
  Header header (data);
  header.datatype() = DataType::from_command_line (DataType::Float32);

  auto out = Image<value_type>::create(argument[2], header);

  // Loop through shells
  FieldUnwrap func (data, field, petable, T);
  ThreadedLoop("unwrapping field", out, 0, 3).run(func, out);

}


