/*******************************************************************************
 * Copyright (c) 2015-2018 Skymind, Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License, Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

//
//  @author sgazeos@gmail.com
//

#include <ops/declarable/helpers/image_suppression.h>
//#include <blas/NDArray.h>

namespace nd4j {
namespace ops {
namespace helpers {

    template <typename T>
    void nonMaxSuppressionV2(NDArray<T>* boxes, NDArray<T>* scales, int maxSize, T threshold, NDArray<T>* output) {
        std::vector<Nd4jLong> indices(scales->lengthOf());
        for (size_t i = 0; i < indices.size(); ++i)
            indices[i] = i;
        std::sort(indices.begin(), indices.end(), [scales](int i, int j) {return (*scales)(i) > (*scales)(j);});

        std::vector<int> selected;
        std::vector<int> selectedIndices(output->lengthOf(), 0);
        auto needToSuppressWithThreshold = [threshold] (NDArray<T>& boxes, int previousIndex, int nextIndex) -> bool {
            T minYPrev = nd4j::math::nd4j_min(boxes(previousIndex, 0), boxes(previousIndex, 2));
            T minXPrev = nd4j::math::nd4j_min(boxes(previousIndex, 1), boxes(previousIndex, 3));
            T maxYPrev = nd4j::math::nd4j_max(boxes(previousIndex, 0), boxes(previousIndex, 2));
            T maxXPrev = nd4j::math::nd4j_max(boxes(previousIndex, 1), boxes(previousIndex, 3));
            T minYNext = nd4j::math::nd4j_min(boxes(nextIndex, 0), boxes(nextIndex, 2));
            T minXNext = nd4j::math::nd4j_min(boxes(nextIndex, 1), boxes(nextIndex, 3));
            T maxYNext = nd4j::math::nd4j_max(boxes(nextIndex, 0), boxes(nextIndex, 2));
            T maxXNext = nd4j::math::nd4j_max(boxes(nextIndex, 1), boxes(nextIndex, 3));
            T areaPrev = (maxYPrev - minYPrev) * (maxXPrev - minXPrev);
            T areaNext = (maxYNext - minYNext) * (maxXNext - minXNext);

            if (areaNext <= T(0.f) || areaPrev <= T(0.f)) return false;

            T minIntersectionY = nd4j::math::nd4j_max(minYPrev, minYNext);
            T minIntersectionX = nd4j::math::nd4j_max(minXPrev, minXNext);
            T maxIntersectionY = nd4j::math::nd4j_min(maxYPrev, maxYNext);
            T maxIntersectionX = nd4j::math::nd4j_min(maxXPrev, maxXNext);
            T intersectionArea =
                    nd4j::math::nd4j_max(maxIntersectionY - minIntersectionY, T(0.0f)) *
                            nd4j::math::nd4j_max(maxIntersectionX - minIntersectionX, T(0.0f));
            T intersectionValue = intersectionArea / (areaPrev + areaNext - intersectionArea);
            return intersectionValue > threshold;

        };
        int numSelected = 0;
        for (int i = 0; i < boxes->sizeAt(0); ++i) {
            if (selected.size() >= output->lengthOf()) break;
            bool shouldSelect = true;
            // Overlapping boxes are likely to have similar scores,
            // therefore we iterate through the selected boxes backwards.
            for (int j = numSelected - 1; j >= 0; --j) {
                if (needToSuppressWithThreshold(*boxes, indices[i], indices[selectedIndices[j]])) {
                    shouldSelect = false;
                    break;
                }
            }
            if (shouldSelect) {
                selected.push_back(indices[i]);
                selectedIndices[numSelected++] = i;
            }
        }
        for (size_t e = 0; e < selected.size(); ++e)
            output->putScalar(e, selected[e]);
    }

    template void nonMaxSuppressionV2(NDArray<float>* boxes, NDArray<float>* scales, int maxSize, float threshold, NDArray<float>* output);
    template void nonMaxSuppressionV2(NDArray<float16>* boxes, NDArray<float16>* scales, int maxSize, float16 threshold, NDArray<float16>* output);
    template void nonMaxSuppressionV2(NDArray<double>* boxes, NDArray<double>* scales, int maxSize, double threshold, NDArray<double>* output);
}
}
}