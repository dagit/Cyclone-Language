/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
package conflux;

import soot.*;
import java.util.*;
import conflux.builder.FlowGraphBuilder;
import conflux.flowgraph.*;

public class ConfluxTransformer extends SceneTransformer {
    static ConfluxTransformer instance = new ConfluxTransformer();

    private ConfluxTransformer() { }

    public static ConfluxTransformer v() { 
	return instance; 
    }

    protected void internalTransform(String phaseName, Map options) {
        final String output_dir = SourceLocator.v().getOutputDir();

	G.v().out.println("Starting ConFLux...");
	
        // Build flow graph
        FlowGraphBuilder builder = new FlowGraphBuilder();
	
	Date startFG = new Date();
	final FlowGraph flowGraph = builder.setup(options);
	builder.build();
	Date endFG = new Date();
	computeTime("Initial flow graph", startFG, endFG);

    }

    protected static void computeTime(String task, Date start, Date end) {
	long time = end.getTime()- start.getTime();
	G.v().out.println("[ConFLux] " + task + " time: " + time/1000 + "." + (time/100)%10 + " seconds.");
    }
}
