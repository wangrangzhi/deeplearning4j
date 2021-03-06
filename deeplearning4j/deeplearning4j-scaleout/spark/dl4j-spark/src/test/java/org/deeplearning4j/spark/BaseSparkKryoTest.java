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

package org.deeplearning4j.spark;

import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaSparkContext;

/**
 * Created by Alex on 04/07/2017.
 */
public class BaseSparkKryoTest extends BaseSparkTest {

    @Override
    public JavaSparkContext getContext() {
        if (sc != null) {
            return sc;
        }

        SparkConf sparkConf = new SparkConf().setMaster("local[" + numExecutors() + "]").setAppName("sparktest");

        sparkConf.set("spark.serializer", "org.apache.spark.serializer.KryoSerializer");
        sparkConf.set("spark.kryo.registrator", "org.nd4j.Nd4jRegistrator");

        sc = new JavaSparkContext(sparkConf);

        return sc;
    }

}

