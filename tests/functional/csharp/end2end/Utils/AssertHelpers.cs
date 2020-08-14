//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    class AssertHelpers
    {
        static public void AssertStringContains(string inputString, string searchString, StringComparison compare = StringComparison.Ordinal)
        {
            Assert.IsTrue(
                inputString?.IndexOf(searchString, compare) >= 0,
                $"Input string does not contain '{searchString}'. Actual: '{inputString}'");
        }
    }
}
